use imgui::*;
use imgui_glium_renderer::Texture;
use std::borrow::Cow;
use std::ops::Add;
use std::ops::Mul;
use std::rc::Rc;
use std::time::Duration;
use std::time::Instant;

use glium::{
    texture::{ClientFormat, RawImage2d},
    uniforms::{MagnifySamplerFilter, MinifySamplerFilter, SamplerBehavior},
    Texture2d,
};

mod support;

const TEX_WIDTH: usize = 200;
const TEX_HEIGHT: usize = 200;
const N_COMPONENTS: usize = TEX_WIDTH * TEX_HEIGHT * 3;

const RAYS_LOWER_LEFT_CORNER: V3 = V3 {
    x: 0.0,
    y: 0.0,
    z: 0.0,
};
const RAYS_HORIZONTAL: V3 = V3 {
    x: 1.0,
    y: 0.0,
    z: 0.0,
};
const RAYS_VERTICAL: V3 = V3 {
    x: 0.0,
    y: 1.0,
    z: 0.0,
};
const RAYS_ORIGIN: V3 = V3 {
    x: 0.0,
    y: 0.0,
    z: 0.0,
};

#[derive(Clone, Copy, Debug)]
struct Ray {
    origin: V3,
    direction: V3,
}

#[derive(Clone, Copy, Debug)]
struct V3 {
    x: f32,
    y: f32,
    z: f32,
}

impl Add for V3 {
    type Output = V3;

    fn add(self, other: V3) -> V3 {
        V3 {
            x: self.x + other.x,
            y: self.y + other.y,
            z: self.z + other.z,
        }
    }
}

impl Mul for V3 {
    type Output = V3;

    fn mul(self, other: V3) -> V3 {
        V3 {
            x: self.x * other.x,
            y: self.y * other.y,
            z: self.z * other.z,
        }
    }
}

impl Mul<f32> for V3 {
    type Output = V3;

    fn mul(self, t: f32) -> V3 {
        V3 {
            x: self.x * t,
            y: self.y * t,
            z: self.z * t,
        }
    }
}

impl Mul<V3> for f32 {
    type Output = V3;

    fn mul(self, v: V3) -> V3 {
        V3 {
            x: self * v.x,
            y: self * v.y,
            z: self * v.z,
        }
    }
}

struct State {
    iteration: i32,
    red: u8,
    blue: u8,
    green: u8,
    checked: bool,
    text: ImString,
    texture: Option<TextureId>,
}

fn compute_color(ray: &Ray, _timestep: f32) -> V3 {
    return ray.direction;
}

fn main() {
    let mut state = State {
        iteration: 0,
        red: 50,
        blue: 50,
        green: 50,
        texture: None,
        checked: false,
        text: ImString::with_capacity(128),
    };

    let system = support::init(file!());

    let mut data: [u8; N_COMPONENTS] = [0; N_COMPONENTS];

    let mut timestep = 0.0;
    let mut main_elapsed: Duration = Duration::new(0, 0);
    system.main_loop(move |_, ui, textures, ctx| {
        state.iteration += 1;

        let main_time = Instant::now();
        let fill_time = Instant::now();

        for i in 0..TEX_WIDTH {
            for j in 0..TEX_HEIGHT {
                let u = (i as f32) / (TEX_WIDTH as f32);
                let v = (j as f32) / (TEX_HEIGHT as f32);

                let ray = Ray {
                    origin: RAYS_ORIGIN,
                    direction: RAYS_LOWER_LEFT_CORNER + u * RAYS_HORIZONTAL + v * RAYS_VERTICAL,
                };

                let color = compute_color(&ray, timestep);

                data[(i * 3) + (j * 3) * TEX_WIDTH + 0] =
                    ((state.red as f32) * (1.0 + color.x)) as u8;
                data[(i * 3) + (j * 3) * TEX_WIDTH + 1] =
                    ((state.blue as f32) * (1.0 + color.y)) as u8;
                data[(i * 3) + (j * 3) * TEX_WIDTH + 2] =
                    ((state.green as f32) * (1.0 + color.z)) as u8;
            }
        }

        let fill_elapsed = fill_time.elapsed();

        let raw = RawImage2d {
            data: Cow::Borrowed(&data),
            width: TEX_WIDTH as u32,
            height: TEX_HEIGHT as u32,
            format: ClientFormat::U8U8U8,
        };

        let gl_texture = Rc::new(Texture2d::new(ctx, raw).unwrap());

        let texture = Texture {
            texture: gl_texture.clone(),
            sampler: SamplerBehavior {
                magnify_filter: MagnifySamplerFilter::Linear,
                minify_filter: MinifySamplerFilter::Linear,
                ..Default::default()
            },
        };

        if let Some(texture_id) = state.texture {
            textures.replace(texture_id, texture);
        } else {
            let texture_id = textures.insert(texture);
            state.texture = Some(texture_id);
        }

        Window::new(im_str!("Settings"))
            .size([300.0, 400.0], Condition::FirstUseEver)
            .position([10.0, 10.0], Condition::FirstUseEver)
            .build(ui, || {
                ui.text(&format!("checked: {}", &state.checked));
                ui.text(&format!("text: {}", &state.text));
                ui.separator();
                ui.text(&format!("fill: {:02} ms", fill_elapsed.as_millis()));
                ui.text(&format!("fill: {:02} ns", fill_elapsed.as_nanos()));
                ui.text(&format!("main: {:02} ms", main_elapsed.as_millis()));
                ui.text(&format!("main: {:02} ns", main_elapsed.as_nanos()));

                ui.separator();
                ui.text(im_str!("Color"));
                Slider::new(im_str!("red"))
                    .range(0..=254)
                    .build(&ui, &mut state.red);
                Slider::new(im_str!("green"))
                    .range(0..=254)
                    .build(&ui, &mut state.green);
                Slider::new(im_str!("blue"))
                    .range(0..=254)
                    .build(&ui, &mut state.blue);
            });

        Window::new(im_str!("Render"))
            .size(
                [TEX_WIDTH as f32 + 100.0, TEX_HEIGHT as f32 + 100.0],
                Condition::FirstUseEver,
            )
            .position([320.0, 10.0], Condition::FirstUseEver)
            .build(ui, || {
                if let Some(texture) = state.texture {
                    Image::new(texture, [TEX_WIDTH as f32, TEX_HEIGHT as f32]).build(ui);
                }
            });

        main_elapsed = main_time.elapsed();
        timestep += 1.0;
    });
}
