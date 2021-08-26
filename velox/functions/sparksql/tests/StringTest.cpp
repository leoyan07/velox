/*
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "velox/functions/sparksql/tests/SparkFunctionBaseTest.h"
#include "velox/type/Type.h"

#include <stdint.h>

namespace facebook::velox::functions::sparksql::test {
namespace {

// This is a five codepoint sequence that renders as a single emoji.
static constexpr char kWomanFacepalmingLightSkinTone[] =
    "\xF0\x9F\xA4\xA6\xF0\x9F\x8F\xBB\xE2\x80\x8D\xE2\x99\x80\xEF\xB8\x8F";

class StringTest : public SparkFunctionBaseTest {
 protected:
  std::optional<int32_t> instr(
      std::optional<std::string> haystack,
      std::optional<std::string> needle) {
    return evaluateOnce<int32_t>("instr(c0, c1)", haystack, needle);
  }

  std::optional<int32_t> length(std::optional<std::string> arg) {
    return evaluateOnce<int32_t>("length(c0)", arg);
  }

  std::optional<int32_t> length_bytes(std::optional<std::string> arg) {
    return evaluateOnce<int32_t, std::string>(
        "length(c0)", {arg}, {VarbinaryType::create()});
  }
};

TEST_F(StringTest, Instr) {
  EXPECT_EQ(instr("SparkSQL", "SQL"), 6);
  EXPECT_EQ(instr(std::nullopt, "SQL"), std::nullopt);
  EXPECT_EQ(instr("SparkSQL", std::nullopt), std::nullopt);
  EXPECT_EQ(instr("SparkSQL", "Spark"), 1);
  EXPECT_EQ(instr("SQL", "SparkSQL"), 0);
  EXPECT_EQ(instr("", ""), 1);
  EXPECT_EQ(instr("abdef", "g"), 0);
  EXPECT_EQ(instr("", "a"), 0);
  EXPECT_EQ(instr("abdef", ""), 1);
  EXPECT_EQ(instr("abc😋def", "😋"), 4);
  // Offsets are calculated in terms of codepoints, not characters.
  // kWomanFacepalmingLightSkinTone is five codepoints.
  EXPECT_EQ(
      instr(std::string(kWomanFacepalmingLightSkinTone) + "abc😋def", "😋"), 9);
  EXPECT_EQ(
      instr(std::string(kWomanFacepalmingLightSkinTone) + "abc😋def", "def"),
      10);
}

TEST_F(StringTest, LengthString) {
  EXPECT_EQ(length(""), 0);
  EXPECT_EQ(length(std::string("\0", 1)), 1);
  EXPECT_EQ(length("1"), 1);
  EXPECT_EQ(length("😋"), 1);
  // Consists of five codepoints.
  EXPECT_EQ(length(kWomanFacepalmingLightSkinTone), 5);
  EXPECT_EQ(length("1234567890abdef"), 15);
}

TEST_F(StringTest, LengthBytes) {
  EXPECT_EQ(length_bytes(""), 0);
  EXPECT_EQ(length_bytes(std::string("\0", 1)), 1);
  EXPECT_EQ(length_bytes("1"), 1);
  EXPECT_EQ(length_bytes("😋"), 4);
  EXPECT_EQ(length_bytes(kWomanFacepalmingLightSkinTone), 17);
  EXPECT_EQ(length_bytes("1234567890abdef"), 15);
}

} // namespace
} // namespace facebook::velox::functions::sparksql::test
