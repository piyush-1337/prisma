#include <bit>
#include <cstddef>
#include <prisma/codecs/audio/wav.hpp>

namespace prisma::codec::wav {

std::expected<FmtChunk, std::string>
parse_fmt_chunk(std::span<const uint8_t> file_data) {
  size_t offset = 0;

  MasterRiffHeader master_header =
      MasterRiffHeader::from_bytes(file_data, offset);

  while (offset < file_data.size()) {
    ChunkHeader chunk_header = ChunkHeader::from_bytes(file_data, offset);

    if (std::memcmp(chunk_header.id, "fmt ", 4) == 0) {
      FmtChunk fmt_chunk = FmtChunk::from_bytes(file_data, offset);

      return fmt_chunk;
    } else {
      offset += (chunk_header.size + 1) & ~1;
    }
  }

  return std::unexpected("fmt chunk not found");
}

std::expected<core::Audio, std::string>
decode(std::span<const uint8_t> file_data) {

  size_t offset = 0;
  MasterRiffHeader master_header =
      MasterRiffHeader::from_bytes(file_data, offset);

  auto fmt_chunk_res = parse_fmt_chunk(file_data);
  if (!fmt_chunk_res)
    return std::unexpected(fmt_chunk_res.error());

  FmtChunk fmt_chunk = *fmt_chunk_res;

  core::Audio audio;
  audio.channels = fmt_chunk.num_channels;
  audio.sample_rate = fmt_chunk.sample_rate;
  audio.bit_depth = fmt_chunk.bits_per_sample;

  while (offset < file_data.size()) {
    ChunkHeader chunk_header = ChunkHeader::from_bytes(file_data, offset);

    if (std::memcmp(chunk_header.id, "data", 4) == 0) {
      audio.pcm.resize(chunk_header.size);
      std::memcpy(audio.pcm.data(), file_data.data() + offset,
                  chunk_header.size);
      return audio;
    } else {
      offset += (chunk_header.size + 1) & ~1;
    }
  }

  return std::unexpected("coudn't find data or fmt chunk");
}

std::expected<WavAudio, std::string> encode(core::Audio audio) {
  MasterRiffHeader master_header;
  master_header.file_size = 4 + sizeof(ChunkHeader) +
                            sizeof(FmtChunk) + sizeof(ChunkHeader) +
                            audio.pcm.size();

  ChunkHeader fmt_chunk_header;
  std::memcpy(&fmt_chunk_header.id, "fmt ", 4);
  fmt_chunk_header.size = sizeof(FmtChunk);

  FmtChunk fmt_chunk;
  fmt_chunk.num_channels = audio.channels;
  fmt_chunk.sample_rate = audio.sample_rate;
  fmt_chunk.bits_per_sample = audio.bit_depth;
  fmt_chunk.block_align = audio.channels * (audio.bit_depth / 8);
  fmt_chunk.byte_rate =
      audio.sample_rate * audio.channels * (audio.bit_depth / 8);

  ChunkHeader data_chunk_header;
  std::memcpy(&data_chunk_header.id, "data", 4);
  data_chunk_header.size = audio.pcm.size();

  WavAudio wav_audio;
  wav_audio.master_header = master_header;
  wav_audio.fmt_chunk_header = fmt_chunk_header;
  wav_audio.fmt_chunk = fmt_chunk;
  wav_audio.data_chunk_header = data_chunk_header;
  wav_audio.pcm = std::move(audio.pcm);

  if constexpr (std::endian::native == std::endian::big) {
    wav_audio.master_header.swap_endianess();
    wav_audio.fmt_chunk_header.swap_endianess();
    wav_audio.fmt_chunk.swap_endianess();
    wav_audio.data_chunk_header.swap_endianess();
  }

  return wav_audio;
}

} // namespace prisma::codec::wav
