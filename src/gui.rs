use std::{env, fs};
use iced::widget::{button, column, text};
use iced::{Element, Sandbox, Settings};

#[derive(Default)]
struct Counter {
    value: i32
}

#[derive(Debug, Clone, Copy)]
enum Message {
    Increment,
    Decrement
}
