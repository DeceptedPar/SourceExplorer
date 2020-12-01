#include "encryption.h"

#include <numeric>

bool encryption_table::init(lak::span<const uint8_t, 0x100> magic_key,
                            const char magic_char)
{
  std::iota(decode_buffer.u32, decode_buffer.u32 + 256, 0);
  // u32 never exceeds the max value of u8 (255)

  auto rotate = [](uint8_t value) -> uint8_t {
    return (value << 7) | (value >> 1);
  };

  uint8_t accum = magic_char;
  uint8_t hash  = magic_char;

  bool never_reset_key = true;

  uint8_t i2 = 0;
  auto *key  = magic_key.begin();
  for (uint32_t i = 0; i < 256; ++i, ++key)
  {
    hash = rotate(hash);

    if (never_reset_key) accum += ((hash & 1 == 0) ? 2 : 3) * *key;

    if (hash == *key)
    {
      if (never_reset_key && !(accum == *(key + 1)))
      {
        ERROR("Failed To Generate Decode Table");
        return false;
      }

      hash = rotate(magic_char);
      key  = magic_key.begin();

      never_reset_key = false;
    }

    i2 += (hash ^ *key) + decode_buffer.u32[i];

    std::swap(decode_buffer.u32[i], decode_buffer.u32[i2]);
  }

  valid = true;
  return true;
}

bool encryption_table::decode(lak::span<uint8_t> chunk) const
{
  if (!valid) return false;

  decode_buffer_t buffer;
  lak::memcpy(&buffer, &decode_buffer);

  uint8_t i  = 0;
  uint8_t i2 = 0;
  for (uint8_t &elem : chunk)
  {
    ++i;
    i2 += (uint8_t)buffer.u32[i];
    std::swap(buffer.u32[i], buffer.u32[i2]);
    elem ^= buffer.u8[(uint8_t)(buffer.u32[i] + buffer.u32[i2]) * 4];
  }
  return true;
}

std::vector<uint8_t> KeyString(const std::u16string &str)
{
  std::vector<uint8_t> result;
  result.reserve(str.size() * 2);
  for (const char16_t code : str)
  {
    if (code & 0xFF) result.emplace_back(code & 0xFF);
    if ((code >> 8) & 0xFF) result.emplace_back((code >> 8) & 0xFF);
  }
  return result;
}
