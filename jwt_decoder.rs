// jwt_decoder.rs
use base64::engine::general_purpose::URL_SAFE_NO_PAD;
use base64::Engine;
use serde_json::Value;
use std::fs;
use std::env;
use termcolor::{Color, ColorChoice, ColorSpec, StandardStream, WriteColor};
use std::io::Write;

const RESET: &str = "\x1b[0m";
const KEY: &str = "\x1b[94m";
const STR: &str = "\x1b[92m";
const NUM: &str = "\x1b[93m";
const BOOL: &str = "\x1b[95m";
const NULL: &str = "\x1b[90m";
const HEADER: &str = "\x1b[96m";
const SIG: &str = "\x1b[90m";

fn base64url_decode(data: &str) -> Result<String, String> {
    let decoded = URL_SAFE_NO_PAD.decode(data).map_err(|e| e.to_string())?;
    String::from_utf8(decoded).map_err(|e| e.to_string())
}

fn colorize_value(v: &Value, indent: usize, level: usize) -> String {
    let spaces = " ".repeat(level * indent);
    match v {
        Value::Object(map) => {
            if map.is_empty() {
                return "{}".to_string();
            }
            let mut lines = vec!["{".to_string()];
            let len = map.len();
            let mut i = 0;
            for (k, val) in map {
                let key_str = format!("{}{}\"{}\"{}: ", KEY, RESET, k, RESET);
                let val_str = colorize_value(val, indent, level + 1);
                let mut line = format!("{}{}{}", spaces, " ".repeat(indent), key_str);
                line.push_str(&val_str);
                if i < len - 1 {
                    line.push(',');
                }
                lines.push(line);
                i += 1;
            }
            lines.push(format!("{}}}", spaces));
            lines.join("\n")
        }
        Value::Array(arr) => {
            if arr.is_empty() {
                return "[]".to_string();
            }
            let mut lines = vec!["[".to_string()];
            for (i, val) in arr.iter().enumerate() {
                let mut line = format!("{}{}{}", spaces, " ".repeat(indent), colorize_value(val, indent, level + 1));
                if i < arr.len() - 1 {
                    line.push(',');
                }
                lines.push(line);
            }
            lines.push(format!("{}]", spaces));
            lines.join("\n")
        }
        Value::String(s) => format!("{}{}\"{}\"{}", STR, RESET, s, RESET),
        Value::Bool(b) => format!("{}{}{}", BOOL, b, RESET),
        Value::Null => format!("{}null{}", NULL, RESET),
        Value::Number(n) => format!("{}{}{}", NUM, n, RESET),
    }
}

fn colorize_json(v: &Value, indent: usize) -> String {
    colorize_value(v, indent, 0)
}

fn decode_jwt(token: &str) -> Result<(Value, Value, String), String> {
    let parts: Vec<&str> = token.split('.').collect();
    if parts.len() != 3 {
        return Err("Неверный JWT: ожидается 3 сегмента".to_string());
    }
    let header_b64 = parts[0];
    let payload_b64 = parts[1];
    let sig = parts[2].to_string();
    let header_str = base64url_decode(header_b64)?;
    let payload_str = base64url_decode(payload_b64)?;
    let header: Value = serde_json::from_str(&header_str).map_err(|e| format!("Ошибка парсинга header: {}", e))?;
    let payload: Value = serde_json::from_str(&payload_str).map_err(|e| format!("Ошибка парсинга payload: {}", e))?;
    Ok((header, payload, sig))
}

fn main() {
    let args: Vec<String> = env::args().collect();
    let mut token = String::new();
    let mut file = None;
    let mut compact = false;

    let mut i = 1;
    while i < args.len() {
        match args[i].as_str() {
            "--file" => {
                if i + 1 < args.len() {
                    file = Some(args[i + 1].clone());
                    i += 2;
                } else { i += 1; }
            }
            "--compact" => {
                compact = true;
                i += 1;
            }
            _ => {
                if token.is_empty() {
                    token = args[i].clone();
                }
                i += 1;
            }
        }
    }
    if let Some(f) = file {
        token = fs::read_to_string(&f).expect("Ошибка чтения файла").trim().to_string();
    }
    if token.is_empty() {
        eprintln!("Не указан JWT-токен.");
        std::process::exit(1);
    }
    let (header, payload, sig) = match decode_jwt(&token) {
        Ok(v) => v,
        Err(e) => {
            eprintln!("Ошибка: {}", e);
            std::process::exit(1);
        }
    };
    let indent = if compact { 0 } else { 2 };
    let mut stdout = StandardStream::stdout(ColorChoice::Auto);
    // Header
    stdout.set_color(ColorSpec::new().set_fg(Some(Color::Cyan))).unwrap();
    writeln!(&mut stdout, "--- Header ---").unwrap();
    stdout.reset().unwrap();
    println!("{}", colorize_json(&header, indent));
    // Payload
    stdout.set_color(ColorSpec::new().set_fg(Some(Color::Cyan))).unwrap();
    writeln!(&mut stdout, "--- Payload ---").unwrap();
    stdout.reset().unwrap();
    println!("{}", colorize_json(&payload, indent));
    // Signature
    stdout.set_color(ColorSpec::new().set_fg(Some(Color::White)).set_intense(false)).unwrap();
    writeln!(&mut stdout, "--- Signature ---").unwrap();
    stdout.reset().unwrap();
    println!("{}", sig);
}
