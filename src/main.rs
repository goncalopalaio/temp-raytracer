use std::time::Duration;
use imgui::*;
use imgui_glium_renderer::Texture;
use std::borrow::Cow;
use std::rc::Rc;
use std::time::Instant;

use glium::{
    texture::{ClientFormat, RawImage2d},
    uniforms::{MagnifySamplerFilter, MinifySamplerFilter, SamplerBehavior},
    Texture2d,
};

mod support;

const TEX_WIDTH: usize = 400;
const TEX_HEIGHT: usize = 400;
const N_COMPONENTS: usize = TEX_WIDTH * TEX_HEIGHT * 3;


struct State {
    iteration: i32,
    red: u8,
    blue: u8,
    green: u8,
    checked: bool,
    text: ImString,
    texture: Option<TextureId>,
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

    let mut main_elapsed: Duration = Duration::new(0, 0);
    system.main_loop(move |_, ui, textures, ctx| {
        state.iteration += 1;

        let main_time = Instant::now();
        let fill_time = Instant::now();

        for i in 0..TEX_WIDTH {
            for j in 0..TEX_HEIGHT {
                // Insert RGB values
                data[(i * 3) + (j * 3) * TEX_WIDTH + 0] = state.red;
                data[(i * 3) + (j * 3) * TEX_WIDTH + 1] = state.blue;
                data[(i * 3) + (j * 3) * TEX_WIDTH + 2] = state.green;
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
    });
}
