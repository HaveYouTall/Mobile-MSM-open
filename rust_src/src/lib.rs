use ark_bls12_377 as bls377;
use ark_ff::fields::Field;
use ark_ff::PrimeField;
use ark_serialize::CanonicalDeserialize;
use ark_serialize::CanonicalSerialize;
use ark_serialize::SerializationError;
use ark_serialize::Write;
use ark_std::rand::Rng;
use ark_std::Zero;
use duration_string::DurationString;
use rand::RngCore;
use rand::SeedableRng;
use rand_chacha::ChaCha20Rng;
use std::cmp;
use std::collections::VecDeque;
use std::fs::File;
use std::time::Duration;
use std::time::Instant;
use thiserror::Error;

#[derive(Debug, Error)]
pub enum HarnessError {
    #[error("could not serialize")]
    SerializationError(#[from] ark_serialize::SerializationError),

    #[error("could not open file")]
    FileOpenError(#[from] std::io::Error),

    #[error("failed to read at least one instance from file")]
    DeserializationError,
}

type Point = bls377::G1Affine;
type Scalar = <bls377::Fr as PrimeField>::BigInt;
type Instance = (Vec<Point>, Vec<Scalar>);

pub fn gen_random_vectors<R: RngCore>(n: usize, rng: &mut R) -> Instance {
    let num_bytes = bls377::Fr::zero().serialized_size();
    let mut points = Vec::<Point>::new();
    let mut scalars = Vec::<Scalar>::new();
    let mut bytes = vec![0; num_bytes];
    let mut scalar;
    for _i in 0..n {
        loop {
            rng.fill_bytes(&mut bytes[..]);
            scalar = bls377::Fr::from_random_bytes(&bytes);
            if scalar.is_some() {
                break;
            }
        }
        scalars.push(scalar.unwrap().into_repr());

        let point: bls377::G1Projective = rng.gen();
        points.push(point.into());
    }
    (points, scalars)
}

pub fn serialize_input(
    dir: &str,
    points: &[Point],
    scalars: &[Scalar],
    append: bool,
) -> Result<(), HarnessError> {
    let points_path = format!("{}{}", dir, "/points");
    let scalars_path = format!("{}{}", dir, "/scalars");
    let (f1, f2) = if append {
        let file1 = File::options()
            .append(true)
            .create(true)
            .open(points_path)?;
        let file2 = File::options()
            .append(true)
            .create(true)
            .open(scalars_path)?;
        (file1, file2)
    } else {
        let file1 = File::create(points_path)?;
        let file2 = File::create(scalars_path)?;
        (file1, file2)
    };
    points.serialize_unchecked(&f1)?;
    scalars.serialize_unchecked(&f2)?;
    Ok(())
}

pub enum FileInputIteratorMode {
    Checked,
    Unchecked,
}

pub struct FileInputIterator {
    points_file: File,
    scalars_file: File,
    mode: FileInputIteratorMode,
    cached: Option<Instance>,
}

impl FileInputIterator {
    pub fn open(dir: &str) -> Result<Self, HarnessError> {
        let points_path = format!("{}{}", dir, "/points");
        let scalars_path = format!("{}{}", dir, "/scalars");

        // Try to read an instance, first in uncheck, then check serialization modes.
        let mut iter = Self {
            points_file: File::open(&points_path)?,
            scalars_file: File::open(&scalars_path)?,
            mode: FileInputIteratorMode::Unchecked,
            cached: None,
        };

        // Read a first value and see if we get a result.
        iter.cached = iter.next();
        if iter.cached.is_some() {
            return Ok(iter);
        }

        let mut iter = Self {
            points_file: File::open(&points_path)?,
            scalars_file: File::open(&scalars_path)?,
            mode: FileInputIteratorMode::Checked,
            cached: None,
        };
        iter.cached = iter.next();
        if iter.cached.is_none() {
            return Err(HarnessError::DeserializationError);
        }
        Ok(iter)
    }
}

impl Iterator for FileInputIterator {
    type Item = Instance;

    fn next(&mut self) -> Option<Self::Item> {
        if self.cached.is_some() {
            return self.cached.take();
        }

        let points = match self.mode {
            FileInputIteratorMode::Checked => Vec::<Point>::deserialize(&self.points_file),
            FileInputIteratorMode::Unchecked => {
                Vec::<Point>::deserialize_unchecked(&self.points_file)
            }
        };

        let points = match points {
            Ok(x) => Some(x),
            Err(_) => None,
        }?;

        let scalars = Vec::<Scalar>::deserialize(&self.scalars_file);
        let scalars = match scalars {
            Ok(x) => Some(x),
            Err(_) => None,
        }?;

        Some((points, scalars))
    }
}

pub struct VectorInputIterator {
    points: VecDeque<Vec<Point>>,
    scalars: VecDeque<Vec<Scalar>>,
}

impl Iterator for VectorInputIterator {
    type Item = Instance;

    fn next(&mut self) -> Option<Self::Item> {
        let points = self.points.pop_front()?;
        let scalars = self.scalars.pop_front()?;
        Some((points, scalars))
    }
}

impl From<Instance> for VectorInputIterator {
    fn from(other: Instance) -> Self {
        Self {
            points: vec![other.0].into(),
            scalars: vec![other.1].into(),
        }
    }
}

impl From<(Vec<Vec<Point>>, Vec<Vec<Scalar>>)> for VectorInputIterator {
    fn from(other: (Vec<Vec<Point>>, Vec<Vec<Scalar>>)) -> Self {
        Self {
            points: other.0.into(),
            scalars: other.1.into(),
        }
    }
}


/// Expose the JNI interface for android below
#[cfg(target_os = "android")]
#[allow(non_snake_case)]
pub mod android {
    extern crate jni;
    use self::jni::objects::{JClass, JString};
    use self::jni::sys::jstring;
    use self::jni::JNIEnv;
    use super::*;
    use rand::thread_rng;
    use std::ffi::CStr;

    #[no_mangle]
    pub unsafe extern "C" fn Java_com_example_mobilemsm_YYteamMSM_genMSMRandom(
        env: JNIEnv,
        _: JClass,
        java_iters: JString,
        java_num_elems: JString,
        java_num_vecs: JString,
    ) {
        let mut rng = thread_rng();
        let mut seed: [u8; 32] = [0; 32];
        rng.fill_bytes(&mut seed[..]);
        let mut chacha_rng = ChaCha20Rng::from_seed(seed);
        let base: i32 = 2;

        let num_elems = env
            .get_string(java_num_elems)
            .expect("invalid string")
            .as_ptr();
        let rust_num_elems = CStr::from_ptr(num_elems).to_str().expect("string invalid");
        let num_elems_val: u32 = rust_num_elems.parse().unwrap();
        let num_elems_exp = base.pow(num_elems_val);

        let num_vecs = env
            .get_string(java_num_vecs)
            .expect("invalid string")
            .as_ptr();
        let rust_num_vecs = CStr::from_ptr(num_vecs).to_str().expect("string invalid");
        let num_vecs_val: u32 = rust_num_vecs.parse().unwrap();

        let iters = env.get_string(java_iters).expect("invalid string").as_ptr();
        let rust_iters = CStr::from_ptr(iters).to_str().expect("string invalid");
        let iters_val: u32 = rust_iters.parse().unwrap();

        let store_dir = "/data/data/com.example.mobilemsm/files/".to_string();
        for _i in 0..num_vecs_val {
            let (points, scalars) =
                gen_random_vectors(num_elems_exp.try_into().unwrap(), &mut chacha_rng);
            serialize_input(&store_dir, &points, &scalars, _i != 0).unwrap();
        }
        
    }

}
