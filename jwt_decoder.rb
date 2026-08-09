# jwt_decoder.rb
require 'base64'
require 'json'
require 'optparse'

RESET = "\033[0m"
KEY = "\033[94m"
STR = "\033[92m"
NUM = "\033[93m"
BOOL = "\033[95m"
NULL = "\033[90m"
HEADER = "\033[96m"
SIG = "\033[90m"

def base64url_decode(data)
  # Добавляем padding
  data += '=' * (4 - data.length % 4) unless data.length % 4 == 0
  Base64.urlsafe_decode64(data)
end

def colorize_value(value, indent, level)
  spaces = ' ' * (level * indent)
  case value
  when Hash
    return "{}" if value.empty?
    lines = ["{"]
    value.each_with_index do |(k, v), i|
      val_str = colorize_value(v, indent, level+1)
      line = spaces + ' ' * indent + "#{KEY}\"#{k}\"#{RESET}: #{val_str}"
      line += "," if i < value.size-1
      lines << line
    end
    lines << spaces + "}"
    lines.join("\n")
  when Array
    return "[]" if value.empty?
    lines = ["["]
    value.each_with_index do |v, i|
      line = spaces + ' ' * indent + colorize_value(v, indent, level+1)
      line += "," if i < value.size-1
      lines << line
    end
    lines << spaces + "]"
    lines.join("\n")
  when String
    "#{STR}\"#{value}\"#{RESET}"
  when TrueClass, FalseClass
    "#{BOOL}#{value}#{RESET}"
  when NilClass
    "#{NULL}null#{RESET}"
  when Numeric
    "#{NUM}#{value}#{RESET}"
  else
    value.to_s
  end
end

def colorize_json(data, indent)
  colorize_value(data, indent, 0)
end

def decode_jwt(token)
  parts = token.split('.')
  raise "Неверный JWT: ожидается 3 сегмента" unless parts.size == 3
  header_b64, payload_b64, sig = parts
  header_str = base64url_decode(header_b64)
  payload_str = base64url_decode(payload_b64)
  header = JSON.parse(header_str)
  payload = JSON.parse(payload_str)
  [header, payload, sig]
end

options = {}
OptionParser.new do |opts|
  opts.banner = "Использование: ruby jwt_decoder.rb <token> [--file FILE] [--compact]"
  opts.on("--file FILE", "Читать токен из файла") { |v| options[:file] = v }
  opts.on("--compact", "Вывод без отступов") { options[:compact] = true }
end.parse!

token = ARGV[0]
if options[:file]
  token = File.read(options[:file]).strip
end
if token.nil? || token.empty?
  $stderr.puts "Не указан JWT-токен."
  exit 1
end

begin
  header, payload, sig = decode_jwt(token)
rescue => e
  $stderr.puts "Ошибка: #{e.message}"
  exit 1
end

indent = options[:compact] ? 0 : 2
puts "#{HEADER}--- Header ---#{RESET}"
puts colorize_json(header, indent)
puts "#{HEADER}--- Payload ---#{RESET}"
puts colorize_json(payload, indent)
puts "#{SIG}--- Signature ---#{RESET}"
puts "#{SIG}#{sig}#{RESET}"
