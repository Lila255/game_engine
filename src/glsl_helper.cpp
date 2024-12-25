#include "glsl_helper.hpp"
#include "chunk.hpp"

namespace glsl_helper
{
	std::array<float, 256 * 4> colours{
		0.0000f, 0.0000f, 0.0000f, 0.0000f, // 0: AIR
		0.1500f, 0.1500f, 0.1500f, 0.6500f, // 1: SMOKE
		0.4000f, 1.0000f, 1.0000f, 0.6500f, // 2: STEAM
		0.1500f, 0.1500f, 0.1500f, 0.5500f, // 3: TEMPORARY_SMOKE
		0.0157f, 1.0000f, 1.0000f, 1.0000f, // 4
		0.0196f, 1.0000f, 1.0000f, 1.0000f, // 5
		0.0235f, 1.0000f, 1.0000f, 1.0000f, // 6
		0.0275f, 1.0000f, 1.0000f, 1.0000f, // 7
		0.0314f, 1.0000f, 1.0000f, 1.0000f, // 8
		0.0353f, 1.0000f, 1.0000f, 1.0000f, // 9
		0.0392f, 1.0000f, 1.0000f, 1.0000f, // 10
		0.0431f, 1.0000f, 1.0000f, 1.0000f, // 11
		0.0471f, 1.0000f, 1.0000f, 1.0000f, // 12
		0.0510f, 1.0000f, 1.0000f, 1.0000f, // 13
		0.0549f, 1.0000f, 1.0000f, 1.0000f, // 14
		0.0588f, 1.0000f, 1.0000f, 1.0000f, // 15
		0.0627f, 1.0000f, 1.0000f, 1.0000f, // 16
		0.0667f, 1.0000f, 1.0000f, 1.0000f, // 17
		0.0706f, 1.0000f, 1.0000f, 1.0000f, // 18
		0.0745f, 1.0000f, 1.0000f, 1.0000f, // 19
		0.0784f, 1.0000f, 1.0000f, 1.0000f, // 20
		0.0824f, 1.0000f, 1.0000f, 1.0000f, // 21
		0.0863f, 1.0000f, 1.0000f, 1.0000f, // 22
		0.0902f, 1.0000f, 1.0000f, 1.0000f, // 23
		0.0941f, 1.0000f, 1.0000f, 1.0000f, // 24
		0.0980f, 1.0000f, 1.0000f, 1.0000f, // 25
		0.1020f, 1.0000f, 1.0000f, 1.0000f, // 26
		0.1059f, 1.0000f, 1.0000f, 1.0000f, // 27
		0.1098f, 1.0000f, 1.0000f, 1.0000f, // 28
		0.1137f, 1.0000f, 1.0000f, 1.0000f, // 29
		0.1176f, 1.0000f, 1.0000f, 1.0000f, // 30
		0.1216f, 1.0000f, 1.0000f, 1.0000f, // 31
		0.0100f, 0.0100f, 0.9800f, 0.7500f, // 32: WATER
		1.0000f, 0.5500f, 0.1000f, 1.0000f, // 33: LAVA
		0.0333f, 1.0000f, 0.0333f, 0.7000f, // 34: ACID
		0.1373f, 1.0000f, 1.0000f, 1.0000f, // 35
		0.1412f, 1.0000f, 1.0000f, 1.0000f, // 36
		0.1451f, 1.0000f, 1.0000f, 1.0000f, // 37
		0.1490f, 1.0000f, 1.0000f, 1.0000f, // 38
		0.1529f, 1.0000f, 1.0000f, 1.0000f, // 39
		0.1569f, 1.0000f, 1.0000f, 1.0000f, // 40
		0.1608f, 1.0000f, 1.0000f, 1.0000f, // 41
		0.1647f, 1.0000f, 1.0000f, 1.0000f, // 42
		0.1686f, 1.0000f, 1.0000f, 1.0000f, // 43
		0.1725f, 1.0000f, 1.0000f, 1.0000f, // 44
		0.1765f, 1.0000f, 1.0000f, 1.0000f, // 45
		0.1804f, 1.0000f, 1.0000f, 1.0000f, // 46
		0.1843f, 1.0000f, 1.0000f, 1.0000f, // 47
		0.1882f, 1.0000f, 1.0000f, 1.0000f, // 48
		0.1922f, 1.0000f, 1.0000f, 1.0000f, // 49
		0.1961f, 1.0000f, 1.0000f, 1.0000f, // 50
		0.2000f, 1.0000f, 1.0000f, 1.0000f, // 51
		0.2039f, 1.0000f, 1.0000f, 1.0000f, // 52
		0.2078f, 1.0000f, 1.0000f, 1.0000f, // 53
		0.2118f, 1.0000f, 1.0000f, 1.0000f, // 54
		0.2157f, 1.0000f, 1.0000f, 1.0000f, // 55
		0.2196f, 1.0000f, 1.0000f, 1.0000f, // 56
		0.2235f, 1.0000f, 1.0000f, 1.0000f, // 57
		0.2275f, 1.0000f, 1.0000f, 1.0000f, // 58
		0.2314f, 1.0000f, 1.0000f, 1.0000f, // 59
		0.2353f, 1.0000f, 1.0000f, 1.0000f, // 60
		0.2392f, 1.0000f, 1.0000f, 1.0000f, // 61
		0.2431f, 1.0000f, 1.0000f, 1.0000f, // 62
		0.2471f, 1.0000f, 1.0000f, 1.0000f, // 63
		0.6000f, 1.0000f, 1.0000f, 0.2000f, // 64: GLASS
		0.0000f, 0.3000f, 0.0500f, 0.8000f, // 65: LEAF
		0.5600f, 0.5600f, 0.4900f, 1.0000f, // 66: STONE
		0.4800f, 0.3600f, 0.2600f, 1.0000f, // 67: DIRT
		0.9300f, 0.8400f, 0.2500f, 1.0000f, // 68: SAND
		0.2700f, 0.8000f, 0.0000f, 1.0000f, // 69: GRASS
		0.3200f, 0.1600f, 0.0000f, 1.0000f, // 70: WOOD
		0.8000f, 0.6700f, 0.0000f, 1.0000f, // 71: GOLD
		0.9988f, 0.1000f, 0.1000f, 1.0000f, // 72: TREE_SEED
		0.4901f, 0.2039f, 0.0549f, 1.0000f, // 73: ROOT
		1.0000f, 0.2352f, 0.0000f, 1.0000f, // 74: EMBER
		0.9000f, 0.9000f, 0.9000f, 1.0000f, // 75: SNOW
		0.9255f, 0.8157f, 0.4039f, 1.0000f, // 76: WAX
		0.3020f, 0.8157f, 0.4039f, 1.0000f, // 77
		0.3059f, 1.0000f, 1.0000f, 1.0000f, // 78
		0.3098f, 1.0000f, 1.0000f, 1.0000f, // 79
		0.3137f, 1.0000f, 1.0000f, 1.0000f, // 80
		0.3176f, 1.0000f, 1.0000f, 1.0000f, // 81
		0.3216f, 1.0000f, 1.0000f, 1.0000f, // 82
		0.3255f, 1.0000f, 1.0000f, 1.0000f, // 83
		0.3294f, 1.0000f, 1.0000f, 1.0000f, // 84
		0.3333f, 1.0000f, 1.0000f, 1.0000f, // 85
		0.3373f, 1.0000f, 1.0000f, 1.0000f, // 86
		0.3412f, 1.0000f, 1.0000f, 1.0000f, // 87
		0.3451f, 1.0000f, 1.0000f, 1.0000f, // 88
		0.3490f, 1.0000f, 1.0000f, 1.0000f, // 89
		0.3529f, 1.0000f, 1.0000f, 1.0000f, // 90
		0.3569f, 1.0000f, 1.0000f, 1.0000f, // 91
		0.3608f, 1.0000f, 1.0000f, 1.0000f, // 92
		0.3647f, 1.0000f, 1.0000f, 1.0000f, // 93
		0.3686f, 1.0000f, 1.0000f, 1.0000f, // 94
		0.3725f, 1.0000f, 1.0000f, 1.0000f, // 95
		0.3765f, 1.0000f, 1.0000f, 1.0000f, // 96
		0.3804f, 1.0000f, 1.0000f, 1.0000f, // 97
		0.3843f, 1.0000f, 1.0000f, 1.0000f, // 98
		0.3882f, 1.0000f, 1.0000f, 1.0000f, // 99
		0.3922f, 1.0000f, 1.0000f, 1.0000f, // 100
		0.3961f, 1.0000f, 1.0000f, 1.0000f, // 101
		0.4000f, 1.0000f, 1.0000f, 1.0000f, // 102
		0.4039f, 1.0000f, 1.0000f, 1.0000f, // 103
		0.4078f, 1.0000f, 1.0000f, 1.0000f, // 104
		0.4118f, 1.0000f, 1.0000f, 1.0000f, // 105
		0.4157f, 1.0000f, 1.0000f, 1.0000f, // 106
		0.4196f, 1.0000f, 1.0000f, 1.0000f, // 107
		0.4235f, 1.0000f, 1.0000f, 1.0000f, // 108
		0.4275f, 1.0000f, 1.0000f, 1.0000f, // 109
		0.4314f, 1.0000f, 1.0000f, 1.0000f, // 110
		0.4353f, 1.0000f, 1.0000f, 1.0000f, // 111
		0.4392f, 1.0000f, 1.0000f, 1.0000f, // 112
		0.4431f, 1.0000f, 1.0000f, 1.0000f, // 113
		0.4471f, 1.0000f, 1.0000f, 1.0000f, // 114
		0.4510f, 1.0000f, 1.0000f, 1.0000f, // 115
		0.4549f, 1.0000f, 1.0000f, 1.0000f, // 116
		0.4588f, 1.0000f, 1.0000f, 1.0000f, // 117
		0.4627f, 1.0000f, 1.0000f, 1.0000f, // 118
		0.4667f, 1.0000f, 1.0000f, 1.0000f, // 119
		0.4706f, 1.0000f, 1.0000f, 1.0000f, // 120
		0.4745f, 1.0000f, 1.0000f, 1.0000f, // 121
		0.4784f, 1.0000f, 1.0000f, 1.0000f, // 122
		0.4824f, 1.0000f, 1.0000f, 1.0000f, // 123
		0.4863f, 1.0000f, 1.0000f, 1.0000f, // 124
		0.4902f, 1.0000f, 1.0000f, 1.0000f, // 125
		0.4941f, 1.0000f, 1.0000f, 1.0000f, // 126
		0.4980f, 1.0000f, 1.0000f, 1.0000f, // 127
		0.5500f, 0.1100f, 0.1100f, 1.0000f, // 128: BRICK_1
		0.7400f, 0.2600f, 0.2100f, 1.0000f, // 129: BRICK_2
		0.6800f, 0.7000f, 0.7400f, 1.0000f, // 130: BRICK_3
		0.5100f, 0.5200f, 0.5500f, 1.0000f, // 131: BRICK_4
		0.6784f, 0.6274f, 0.5843f, 1.0000f, // 132: Mortar
		0.3568f, 0.8078f, 0.9803f, 1.0000f, // 133: TRANS_BLUE
		0.9607f, 0.6627f, 0.7215f, 1.0000f, // 134: TRANS_PINK
		1.0000f, 1.0000f, 1.0000f, 1.0000f, // 135: WHITE
		0.9294f, 0.7882f, 0.2588f, 1.0000f, // 136: BEE_YELLOW
		0.1000f, 0.1000f, 0.1000f, 1.0000f, // 137: BEE_BLACK
		0.5412f, 1.0000f, 1.0000f, 1.0000f, // 138
		0.5451f, 1.0000f, 1.0000f, 1.0000f, // 139
		0.5490f, 1.0000f, 1.0000f, 1.0000f, // 140
		0.5529f, 1.0000f, 1.0000f, 1.0000f, // 141
		0.5569f, 1.0000f, 1.0000f, 1.0000f, // 142
		0.5608f, 1.0000f, 1.0000f, 1.0000f, // 143
		0.5647f, 1.0000f, 1.0000f, 1.0000f, // 144
		0.5686f, 1.0000f, 1.0000f, 1.0000f, // 145
		0.5725f, 1.0000f, 1.0000f, 1.0000f, // 146
		0.5765f, 1.0000f, 1.0000f, 1.0000f, // 147
		0.5804f, 1.0000f, 1.0000f, 1.0000f, // 148
		0.5843f, 1.0000f, 1.0000f, 1.0000f, // 149
		0.5882f, 1.0000f, 1.0000f, 1.0000f, // 150
		0.5922f, 1.0000f, 1.0000f, 1.0000f, // 151
		0.5961f, 1.0000f, 1.0000f, 1.0000f, // 152
		0.6000f, 1.0000f, 1.0000f, 1.0000f, // 153
		0.6039f, 1.0000f, 1.0000f, 1.0000f, // 154
		0.6078f, 1.0000f, 1.0000f, 1.0000f, // 155
		0.6118f, 1.0000f, 1.0000f, 1.0000f, // 156
		0.6157f, 1.0000f, 1.0000f, 1.0000f, // 157
		0.6196f, 1.0000f, 1.0000f, 1.0000f, // 158
		0.6235f, 1.0000f, 1.0000f, 1.0000f, // 159
		0.6275f, 1.0000f, 1.0000f, 1.0000f, // 160
		0.6314f, 1.0000f, 1.0000f, 1.0000f, // 161
		0.6353f, 1.0000f, 1.0000f, 1.0000f, // 162
		0.6392f, 1.0000f, 1.0000f, 1.0000f, // 163
		0.6431f, 1.0000f, 1.0000f, 1.0000f, // 164
		0.6471f, 1.0000f, 1.0000f, 1.0000f, // 165
		0.6510f, 1.0000f, 1.0000f, 1.0000f, // 166
		0.6549f, 1.0000f, 1.0000f, 1.0000f, // 167
		0.6588f, 1.0000f, 1.0000f, 1.0000f, // 168
		0.6627f, 1.0000f, 1.0000f, 1.0000f, // 169
		0.6667f, 1.0000f, 1.0000f, 1.0000f, // 170
		0.6706f, 1.0000f, 1.0000f, 1.0000f, // 171
		0.6745f, 1.0000f, 1.0000f, 1.0000f, // 172
		0.6784f, 1.0000f, 1.0000f, 1.0000f, // 173
		0.6824f, 1.0000f, 1.0000f, 1.0000f, // 174
		0.6863f, 1.0000f, 1.0000f, 1.0000f, // 175
		0.6902f, 1.0000f, 1.0000f, 1.0000f, // 176
		0.6941f, 1.0000f, 1.0000f, 1.0000f, // 177
		0.6980f, 1.0000f, 1.0000f, 1.0000f, // 178
		0.7020f, 1.0000f, 1.0000f, 1.0000f, // 179
		0.7059f, 1.0000f, 1.0000f, 1.0000f, // 180
		0.7098f, 1.0000f, 1.0000f, 1.0000f, // 181
		0.7137f, 1.0000f, 1.0000f, 1.0000f, // 182
		0.7176f, 1.0000f, 1.0000f, 1.0000f, // 183
		0.7216f, 1.0000f, 1.0000f, 1.0000f, // 184
		0.7255f, 1.0000f, 1.0000f, 1.0000f, // 185
		0.7294f, 1.0000f, 1.0000f, 1.0000f, // 186
		0.7333f, 1.0000f, 1.0000f, 1.0000f, // 187
		0.7373f, 1.0000f, 1.0000f, 1.0000f, // 188
		0.7412f, 1.0000f, 1.0000f, 1.0000f, // 189
		0.7451f, 1.0000f, 1.0000f, 1.0000f, // 190
		0.7490f, 1.0000f, 1.0000f, 1.0000f, // 191
		0.7529f, 1.0000f, 1.0000f, 1.0000f, // 192
		0.7569f, 1.0000f, 1.0000f, 1.0000f, // 193
		0.7608f, 1.0000f, 1.0000f, 1.0000f, // 194
		0.7647f, 1.0000f, 1.0000f, 1.0000f, // 195
		0.7686f, 1.0000f, 1.0000f, 1.0000f, // 196
		0.7725f, 1.0000f, 1.0000f, 1.0000f, // 197
		0.7765f, 1.0000f, 1.0000f, 1.0000f, // 198
		0.7804f, 1.0000f, 1.0000f, 1.0000f, // 199
		0.7843f, 1.0000f, 1.0000f, 1.0000f, // 200
		0.7882f, 1.0000f, 1.0000f, 1.0000f, // 201
		0.7922f, 1.0000f, 1.0000f, 1.0000f, // 202
		0.7961f, 1.0000f, 1.0000f, 1.0000f, // 203
		0.8000f, 1.0000f, 1.0000f, 1.0000f, // 204
		0.8039f, 1.0000f, 1.0000f, 1.0000f, // 205
		0.8078f, 1.0000f, 1.0000f, 1.0000f, // 206
		0.8118f, 1.0000f, 1.0000f, 1.0000f, // 207
		0.8157f, 1.0000f, 1.0000f, 1.0000f, // 208
		0.8196f, 1.0000f, 1.0000f, 1.0000f, // 209
		0.8235f, 1.0000f, 1.0000f, 1.0000f, // 210
		0.8275f, 1.0000f, 1.0000f, 1.0000f, // 211
		0.8314f, 1.0000f, 1.0000f, 1.0000f, // 212
		0.8353f, 1.0000f, 1.0000f, 1.0000f, // 213
		0.8392f, 1.0000f, 1.0000f, 1.0000f, // 214
		0.8431f, 1.0000f, 1.0000f, 1.0000f, // 215
		0.8471f, 1.0000f, 1.0000f, 1.0000f, // 216
		0.8510f, 1.0000f, 1.0000f, 1.0000f, // 217
		0.8549f, 1.0000f, 1.0000f, 1.0000f, // 218
		0.8588f, 1.0000f, 1.0000f, 1.0000f, // 219
		0.8627f, 1.0000f, 1.0000f, 1.0000f, // 220
		0.8667f, 1.0000f, 1.0000f, 1.0000f, // 221
		0.8706f, 1.0000f, 1.0000f, 1.0000f, // 222
		0.8745f, 1.0000f, 1.0000f, 1.0000f, // 223
		0.8784f, 1.0000f, 1.0000f, 1.0000f, // 224
		0.8824f, 1.0000f, 1.0000f, 1.0000f, // 225
		0.8863f, 1.0000f, 1.0000f, 1.0000f, // 226
		0.8902f, 1.0000f, 1.0000f, 1.0000f, // 227
		0.8941f, 1.0000f, 1.0000f, 1.0000f, // 228
		0.8980f, 1.0000f, 1.0000f, 1.0000f, // 229
		0.9020f, 1.0000f, 1.0000f, 1.0000f, // 230
		0.9059f, 1.0000f, 1.0000f, 1.0000f, // 231
		0.9098f, 1.0000f, 1.0000f, 1.0000f, // 232
		0.9137f, 1.0000f, 1.0000f, 1.0000f, // 233
		0.9176f, 1.0000f, 1.0000f, 1.0000f, // 234
		0.9216f, 1.0000f, 1.0000f, 1.0000f, // 235
		0.9255f, 1.0000f, 1.0000f, 1.0000f, // 236
		0.9294f, 1.0000f, 1.0000f, 1.0000f, // 237
		0.9333f, 1.0000f, 1.0000f, 1.0000f, // 238
		0.9373f, 1.0000f, 1.0000f, 1.0000f, // 239
		0.9412f, 1.0000f, 1.0000f, 1.0000f, // 240
		0.9451f, 1.0000f, 1.0000f, 1.0000f, // 241
		0.9490f, 1.0000f, 1.0000f, 1.0000f, // 242
		0.9529f, 1.0000f, 1.0000f, 1.0000f, // 243
		0.9569f, 1.0000f, 1.0000f, 1.0000f, // 244
		0.9608f, 1.0000f, 1.0000f, 1.0000f, // 245
		0.9647f, 1.0000f, 1.0000f, 1.0000f, // 246
		0.9686f, 1.0000f, 1.0000f, 1.0000f, // 247
		0.9725f, 1.0000f, 1.0000f, 1.0000f, // 248
		0.9765f, 1.0000f, 1.0000f, 1.0000f, // 249
		0.9804f, 1.0000f, 1.0000f, 1.0000f, // 250
		0.9843f, 1.0000f, 1.0000f, 1.0000f, // 251
		0.9882f, 1.0000f, 1.0000f, 1.0000f, // 252
		0.9922f, 1.0000f, 1.0000f, 1.0000f, // 253
		0.9961f, 1.0000f, 1.0000f, 1.0000f, // 254
		1.0000f, 1.0000f, 1.0000f, 1.0000f, // 255
	};

	std::unordered_map<std::string, GLuint> texture_map;

	std::string frag_0()
	{
		return
			R"(
			#version 430
			in vec2 v_TexCoord;
			
			layout(binding = 0, r8ui) uniform readonly uimage2D tex;
			uniform ivec2 texture_size; 
			uniform vec4 colours[255];
			out vec4 out_Color;

			void main()
			{
				uint value = imageLoad(tex, ivec2(v_TexCoord.xy * vec2(texture_size))).r;
				out_Color = colours[value];
			}

			)";

		// return frag_0.str();
	}

	std::string vert_0()
	{
		return R"(
			#version 430
			layout (location = 0) in vec3 in_Position;
			layout (location = 1) in vec2 in_TexCoord;
			uniform mat4 projection;
			uniform mat4 view;
			out vec2 v_TexCoord;
			void main()
			{
				v_TexCoord = in_TexCoord;
				gl_Position = projection * view * vec4(in_Position, 1.0);
				// gl_Position = vec4(in_Position, 1.0);
				// gl_Position = projection * vec4(in_Position, 1.0);
			}
		)";
	}

	std::string vert_1()
	{
		std::stringstream vert_1;
		vert_1
			<< "#version 430\n"
			<< "layout (location = 0) in vec3 in_Position;\n"
			<< "uniform mat4 projection;\n"
			<< "uniform mat4 view;\n"
			<< "void main()\n"
			<< "{\n"
			<< "gl_Position = projection * view * vec4(in_Position, 1.0);\n"
			<< "}\n";
		return vert_1.str();
	}

	// Output white
	std::string frag_1()
	{
		std::stringstream frag_1;
		frag_1
			<< "#version 430\n"
			<< "out vec4 out_Color;\n"
			<< "void main()\n"
			<< "{\n"
			<< "out_Color = vec4(1.0, 1.0, 1.0, 1.0);\n"
			<< "}\n";
		return frag_1.str();
	}

	// Output texture
	std::string vert_2()
	{
		return
			R"(
			#version 430
			layout (location = 0) in vec3 in_Position;
			layout (location = 1) in vec2 texCoord;
			uniform mat4 projection;
			uniform mat4 view;
			//uniform ivec2 texture_size; 
			out vec2 v_TexCoord;
			void main()
			{
				v_TexCoord = texCoord;// * vec2(texture_size);
				gl_Position = projection * view * vec4(in_Position, 1.0);
			}
		)";
	}

	std::string frag_2()
	{
		return
			R"(
			#version 430
			in vec2 v_TexCoord;
			// uniform usampler2D tex;
			layout(binding = 0, r32ui) uniform readonly uimage2D tex;
			layout(binding = 1, r32ui) uniform readonly uimage2D blurred_tex;
			// layout(binding = 2, r32ui) uniform uimage2D colour_tex;
			layout(binding = 2, r8ui) uniform readonly uimage2D world_chunks;
			uniform ivec2 texture_size; 
			out vec4 out_Color;

			uint sampleWorld(vec2 texCoord) {
				if (texCoord.x < 0.0 || texCoord.x >= texture_size.x || texCoord.y < 0.0 || texCoord.y >= texture_size.y) {
					return -1;	// -1 is out of bounds, uint max value so doing value > 0 will work
				}
				uint value = imageLoad(world_chunks, ivec2(texCoord.xy)).r;
				return value;
			}

			vec3 hsv2rgb(float h) {
				float c = 1.0;
				float x = c * (1.0 - abs(mod(h / 60.0, 2.0) - 1.0));
				float m = 0.0;

				vec3 rgb;
				if (h < 60.0) {
					rgb = vec3(c, x, 0.0);
				} else if (h < 120.0) {
					rgb = vec3(x, c, 0.0);
				} else if (h < 180.0) {
					rgb = vec3(0.0, c, x);
				} else if (h < 240.0) {
					rgb = vec3(0.0, x, c);
				} else if (h < 300.0) {
					rgb = vec3(x, 0.0, c);
				} else {
					rgb = vec3(c, 0.0, x);
				}

				return rgb + m;
			}

			void main()
			{
				// out_Color = vec4(val >> 24, (val >> 16) & 0xFF, (val >> 8) & 0xFF, val & 0xFF) / 255.0;

				uint value = imageLoad(tex, ivec2(v_TexCoord * texture_size)).r;
				uint blurred_value = imageLoad(blurred_tex, ivec2(v_TexCoord * texture_size)).r;

				// uint hue_value = imageLoad(colour_tex, ivec2(v_TexCoord * texture_size)).r;
				// float hue = (float(hue_value) / 4294967295.0) * 360.0;
				// vec3 rgb = hsv2rgb(hue);
				float inverse_alpha = 0.0;
				uint world_value = sampleWorld(v_TexCoord * texture_size);
				if(world_value < 32) {
					// inverse_alpha = value / (6.0 *  16000);// + blurred_value / 512000.0;
					inverse_alpha = value / (4.0 *  16000) + blurred_value / 1896000.0;
				} else {
					inverse_alpha = blurred_value / (2.5 *  16000);
				}
				
				inverse_alpha = max(0.0, inverse_alpha);
				inverse_alpha = min(1.0, inverse_alpha);

				// inverse_alpha = inverse_alpha * inverse_alpha;


				float alpha = 1.0 - inverse_alpha;
				// alpha = sin(1.5708 * alpha);
				// alpha = 1 - 1.0 / (12 * alpha + 1.0) + 1 / 12.0;
				// alpha = alpha * alpha;
				// alpha = alpha * alpha * alpha;
				// alpha = (alpha * alpha + alpha) / 2.0;



				// alpha = alpha * alpha;

				out_Color = vec4(0, 0, 0, alpha);
			}
		)";
	}

	std::string light_compute_shader()
	{
		return R"(
			#version 430
			layout(binding = 0, r32ui) uniform uimage2D lightingTex; // 3x3 grid of 128x128 textures, lays on top of worldChunkTex textures
			
			// layout(binding = 0, rg32ui) uniform coherent uimage2D lightingTex;
			layout(binding = 1, r8ui) uniform readonly uimage2D world_chunks;  // 3x3 grid of 128x128 textures, lays on top of worldChunkTex textures

			const int CHUNK_SIZE = 64;  // width and height of a chunk
			const int CHUNKS_WIDTH = 3;	// number of chunks in a row

			uniform ivec2 texture_size; // size of the lighting texture
			uniform uint frame_count;

			// layout(binding = 2) uniform NormalVectors {
    		// 	vec2 data[256];
			// } normal_vectors;
			//uniform vec2 normal_vectors[256];
			layout(binding = 2, rg32f) uniform readonly image1D normal_vectors;
			
			// layout(binding = 3, r32ui) uniform uimage2D lighting_colour_tex;

			// texCoord is position in lightingTex
			uint sampleWorld(vec2 texCoord) {
				if (texCoord.x < 0.0 || texCoord.x >= texture_size.x || texCoord.y < 0.0 || texCoord.y >= texture_size.y) {
					return -1;	// -1 is out of bounds, uint max value so doing value > 0 will work
				}
				uint value = imageLoad(world_chunks, ivec2(texCoord.xy)).r;
				return value;
			}

			vec2 refract_ray(vec2 ray_dir, float current_ior, float next_ior)
			{
				// float cosThetaI = dot(ray_dir, vec2(0.0, 1.0));
				// float sinThetaI = sqrt(1.0 - cosThetaI * cosThetaI);
				// float sinThetaT = (current_ior / next_ior) * sinThetaI;
				// float cosThetaT = sqrt(1.0 - sinThetaT * sinThetaT);
				// // return vec2(-cosThetaT, sinThetaT);
				// // return normalize(vec2(-ray_dir.x, -ray_dir.y * cosThetaI / cosThetaR));
				// return normalize(vec2(-ray_dir.x * cosThetaT, -ray_dir.y * sinThetaT));
				// // return ray_dir;
				float cosThetaI = dot(normalize(ray_dir), vec2(0.0, 1.0));
				float sinThetaI = sqrt(1.0 - cosThetaI * cosThetaI);
				float sinThetaT = (current_ior / next_ior) * sinThetaI;
				float cosThetaT = sqrt(1.0 - sinThetaT * sinThetaT);

				// Handle total internal reflection
				if (sinThetaT > 1.0) {
					return reflect(ray_dir, vec2(0.0, 1.0));
				}

				return normalize(vec2(-ray_dir.x * cosThetaT, -ray_dir.y * sinThetaT));
			}


			uniform vec2 player_pos;    // relative to the lighting texture, center of the screen but not center of lighting texture
			float step_distance = .919191911919;
			
			const int max_ray_length = 512;
			// const float ior_values[6] = float[6](1.0, 1.01, 1.33, 1.52, 1.62, 1.65);

			float random(float seed) {
			    return fract(sin(seed) * 43758.5453123);
			}

			// raycast from player_pos to the edge of the screen,  16000 invocations
			layout(local_size_x = 1, local_size_y = 1) in;
			void main() {
				if(player_pos.x < 0.0 || player_pos.x > texture_size.x || player_pos.y < 0.0 || player_pos.y > texture_size.y) {
					return;
				}
				int ray_index = int(gl_GlobalInvocationID.x);
				float ray_decimal = float(ray_index) / float( 16000);
				float ray_angle = ((float(ray_index) + random(ray_decimal)) / float( 16000)) * 2.0 * 3.1415926535897932384626433832795;
				// uint hue_val = uint((float(ray_index) /  16000.0) * 4294967295.0);
				vec2 ray_dir = vec2(cos(ray_angle), sin(ray_angle));

				vec2 ray_pos = player_pos;
				
				uint bounces = 0;
				uint metal_bounces = 0;
				uint refracted_bounces = 0;

				// if (sampleWorld(ray_pos) > 0 && sampleWorld(ray_pos) < 64) {
				// 	refracted_bounces = 1;
				// }

				// loop until we hit something or we reach max_ray_length
				for (int i = 0; i < max_ray_length;) {
					// ivec2 ray_pos_int = ivec2(ray_pos);
					uint sample_v0 = sampleWorld(ray_pos);
					// while (ray_pos_int == ivec2(ray_pos)) { // if we're still in the same pixel, move forward a bit
					ray_pos += step_distance * ray_dir;
					i++;
					// }

					if (sample_v0 > 0 && sample_v0 < 64) {
						refracted_bounces = 1;
					}

					// ray_pos += ray_dir;
					uint sample_v = sampleWorld(ray_pos);
					
					// if(sample_v == 1 && sample_v0 != 1)
					// {
					// 	refracted_bounces = 1;
					// }
					// else if(sample_v == 2)
					// {
					// 	refracted_bounces = 1;
						
					// 	// hit water, refract
					// 	// ray_dir = refract_ray(ray_dir, 1.0003, 1.33);
					//	if(sample_v0 != sample_v) 
					//	{
					//		ray_dir = refract_ray(ray_dir, ior_values[sample_v0], ior_values[sample_v]);
					//	}
					// 		ray_dir = refract_ray(ray_dir, 1.0003, 1.33);
					// 		ray_pos += step_distance * ray_dir;
					// }
					// } else

					if (sample_v >= 64) {     // hit something solid

						if (sample_v < 66)	// can pass through
						{
							// if (sample_v0 != sample_v) {
							// 	ray_dir = refract_ray(ray_dir, 1.00, 1.10);
							// }
						}
						else {

							uint surround_values = 0;
							// for(int j = 0; j < 9; j++)
							// {
							// 	if(j == 4) continue; // skip center
							// 	int x = j % 3;
							// 	int y = j / 3;
							// 	uint world_sample = sampleWorld(ray_pos + vec2(x - 1, y - 1));
							// 	if(world_sample > 0) {
							// 		uint bit_to_shift_by = j;
							// 		if(j > 4) bit_to_shift_by--;
							// 		surround_values |= 1 << (8-bit_to_shift_by);
							// 	}
							// }
							uint counter = 0;
							for(int y = 0; y < 3; y++)
							{
								for(int x = 0; x < 3; x++)
								{
									if(x == 1 && y == 1) continue;
									counter++;
									uint world_sample = sampleWorld(ray_pos + vec2(x - 1, y - 1));
									if(world_sample > 0) {
										surround_values |= 1 << (8-counter);
									}

								}
							}

							vec2 normal_vec = imageLoad(normal_vectors, int(surround_values)).rg;

							float dot_val = dot(normal_vec, ray_dir);

							if(dot_val < 0.0) {
								normal_vec = -normal_vec;
								dot_val = -dot_val;
							}
							vec2 reflection = ray_dir - 2.0 * dot_val * normal_vec;


							// Add a small random variation
							float noise_scale = 0.69; 
							reflection += noise_scale * ((ray_index % 100 - 50) / float(50.0));

							vec2 rrr = normalize(reflection);

							ray_dir = rrr;
							ray_pos += .5 * ray_dir;
							if(sampleWorld(ray_pos) >= 32) {
								break;
							}
							bounces++;
							i++;
						}
					}
					// if(bounces > 8) {
					// 	break;
					// }

					// imageAtomicExchange(lighting_colour_tex, ivec2(ray_pos.xy), hue_val);
					
					// imageAtomicAdd(lightingTex,  ivec2(ray_pos.xy),  6 * int(bounces > 0)); 

					// imageAtomicAdd(lightingTex,  ivec2(ray_pos.xy),  uint(8.0 / (bounces / 2.0 + 2))); 
					
					// imageAtomicAdd(lightingTex,  ivec2(ray_pos.xy),  1); 
					
					// imageAtomicAdd(lightingTex,  ivec2(ray_pos.xy),  bounces > 0 ? 0.5 : 1); 
					// if(bounces == 1)
					if(bounces > 0)
					{
						imageAtomicAdd(lightingTex,  ivec2(ray_pos.xy), 5 - 2 * refracted_bounces); 
						// imageAtomicAdd(lightingTex,  ivec2(ray_pos.xy), 5 + metal_bounces -  2 * refracted_bounces); 
					} else {
						imageAtomicAdd(lightingTex,  ivec2(ray_pos.xy), 5 - 2 * refracted_bounces);
					}
				}
			};
		)";
	}

	std::string light_blending_compute_shader()
	{
		return R"(
			#version 430
			layout(binding = 0, r32ui) uniform uimage2D blended_lights;
			layout(binding = 1, r32ui) uniform uimage2D new_lights;
			layout(binding = 2, r32ui) uniform uimage2D old_lights;
			uniform int total_frames;

			layout(local_size_x = 1, local_size_y = 1) in;

			void main() {
				ivec2 texel = ivec2(gl_GlobalInvocationID.xy);
				uvec4 new_light = imageLoad(new_lights, texel);
				uvec4 blended_light = imageLoad(blended_lights, texel);

				uvec4 old_light = imageLoad(old_lights, texel);

				// blended_light -= uvec4(vec4(old_light) / total_frames);
				// blended_light += uvec4(vec4(new_light) / total_frames);
				blended_light -= old_light;// * uvec4(1,1,1, .1);
				blended_light += new_light;// * uvec4(1,1,1, .1);
				
				imageStore(blended_lights, texel, blended_light);
			}

		)";
	}

	std::string light_blurring_compute_shader()
	{
		return R"(
			#version 430
			layout(binding = 0, r32ui) uniform uimage2D blurred_lights;
			layout(binding = 1, r32ui) uniform uimage2D og_lights;

			#define KERNEL_SIZE 9
			const float kernel[KERNEL_SIZE] = float[KERNEL_SIZE](
				0.0625, 0.125, 0.0625,
				0.125,  0.25,  0.125,
				0.0625, 0.125, 0.0625
			);

			layout(local_size_x = 1, local_size_y = 1) in;
			void main() {
				ivec2 texel = ivec2(gl_GlobalInvocationID.xy);
				vec4 result = vec4(0.0);
				
				for(int i = 0; i < KERNEL_SIZE; i++) {
					for(int j = 0; j < KERNEL_SIZE; j++) {
						ivec2 offset = ivec2(i - KERNEL_SIZE / 2, j - KERNEL_SIZE / 2);
						result += imageLoad(og_lights, texel + offset) * kernel[i] * kernel[j];
					}
				}
				
				imageStore(blurred_lights, texel, uvec4(result));
			}

		)";
	}

	GLuint load_shader(const std::string &source, GLenum type)
	{
		printf("load_shader_0: %d\n", glGetError());
		GLuint shader = glCreateShader(type);
		const char *src = source.c_str();
		glShaderSource(shader, 1, &src, nullptr);
		printf("load_shader_1: %d\n", glGetError());
		glCompileShader(shader);
		printf("load_shader_2: %d\n", glGetError());

		GLint isCompiled = 0;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
		printf("load_shader_3: %d\n", glGetError());
		if (isCompiled == GL_FALSE)
		{
			std::printf("Failed to compile shader %d\n", glGetError());

			// Get shader compilation log
			int maxLength = 0;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

			std::vector<char> errorLog(maxLength);
			glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);

			// Print error log
			std::printf("Failed to compile shader: %s\n", &errorLog[0]);

			return 0;
		}
		return shader;
	}

	void create_character_texture(GLuint &texture)
	{
		// Create a texture for the character
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		// Set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// // Set texture filtering
		// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// Set texture data
		// std::array<std::array<uint8_t, 3>, 8> data = {
		//     std::array<uint8_t, 3>{101, 100, 101},
		//     std::array<uint8_t, 3>{0, 100, 0},
		//     std::array<uint8_t, 3>{100, 100, 0},
		//     std::array<uint8_t, 3>{100, 100, 100},
		//     std::array<uint8_t, 3>{100, 0, 100},
		//     std::array<uint8_t, 3>{100, 100, 100},
		//     std::array<uint8_t, 3>{0, 100, 101},
		//     std::array<uint8_t, 3>{100, 101, 100},
		// };
		// std::array<std::array<uint8_t, 8>, 3> data = {
		//     std::array<uint8_t, 8>{101, 100, 101, 0, 100, 0, 100, 100},
		//     std::array<uint8_t, 8>{100, 100, 100, 100, 0, 100, 100, 100},
		//     std::array<uint8_t, 8>{100, 101, 100, 0, 100, 101, 100, 101},
		// };
		std::array<uint8_t, character_height * character_width> data = {
			66, 66, 66, 66,
			66, 68, 68, 66,
			66, 68, 68, 66,
			66, 68, 68, 66,
			66, 69, 69, 66,
			0, 69, 69, 0,
			0, 69, 69, 0,
			66, 69, 69, 66,
			66, 68, 68, 66,
			66, 68, 68, 66,
			66, 68, 68, 66,
			66, 66, 66, 66};

		// 66, 66, 66, 66,
		// 66, 64, 64, 66,
		// 66, 64, 64, 66,
		// 66, 64, 64, 66,
		// 66, 64, 64, 66,
		// 0, 64, 64, 0,
		// 0, 64, 64, 0,
		// 66, 64, 64, 66,
		// 66, 64, 64, 66,
		// 66, 64, 64, 66,
		// 66, 64, 64, 66,
		// 66, 66, 66, 66};

		// game::tile_type::TRANS_BLUE, game::tile_type::TRANS_BLUE, game::tile_type::TRANS_BLUE, game::tile_type::TRANS_BLUE,
		// game::tile_type::TRANS_BLUE, game::tile_type::TRANS_BLUE, game::tile_type::TRANS_BLUE, game::tile_type::TRANS_BLUE,
		// game::tile_type::TRANS_BLUE, game::tile_type::TRANS_BLUE, game::tile_type::TRANS_BLUE, game::tile_type::TRANS_BLUE,
		// game::tile_type::TRANS_PINK, game::tile_type::TRANS_PINK, game::tile_type::TRANS_PINK, game::tile_type::TRANS_PINK,
		// game::tile_type::TRANS_PINK, game::tile_type::TRANS_PINK, game::tile_type::TRANS_PINK, game::tile_type::TRANS_PINK,
		// game::tile_type::WHITE, game::tile_type::WHITE, game::tile_type::WHITE, game::tile_type::WHITE,
		// game::tile_type::WHITE, game::tile_type::WHITE, game::tile_type::WHITE, game::tile_type::WHITE,
		// game::tile_type::TRANS_PINK, game::tile_type::TRANS_PINK, game::tile_type::TRANS_PINK, game::tile_type::TRANS_PINK,
		// game::tile_type::TRANS_PINK, game::tile_type::TRANS_PINK, game::tile_type::TRANS_PINK, game::tile_type::TRANS_PINK,
		// game::tile_type::TRANS_BLUE, game::tile_type::TRANS_BLUE, game::tile_type::TRANS_BLUE, game::tile_type::TRANS_BLUE,
		// game::tile_type::TRANS_BLUE, game::tile_type::TRANS_BLUE, game::tile_type::TRANS_BLUE, game::tile_type::TRANS_BLUE,
		// game::tile_type::TRANS_BLUE, game::tile_type::TRANS_BLUE, game::tile_type::TRANS_BLUE, game::tile_type::TRANS_BLUE};

		// 4,
		// 5,
		// 6,
		// 7,
		// 4,
		// 5,
		// 6,
		// 7,
		// 4,
		// 5,
		// 61,
		// 37,
		// 154,
		// 52,
		// 26,
		// 7,
		// 42,
		// 5,
		// 56,
		// 7,
		// 49,
		// 56,
		// 69,
		// 74,
		// 34,
		// 54,
		// 66,
		// 7,
		// 4,
		// 5,
		// 6,
		// 87,
		// 47,
		// 43,
		// 96,
		// 78,
		// 46,
		// 51,
		// 63,
		// 77,
		// 49,
		// 50,
		// 61,
		// 17,
		// 24,
		// 15,
		// 16,
		// 17};
		// std::array<uint8_t, 24> data = {
		// 	101,
		// 	100,
		// 	101,
		// 	1,
		// 	100,
		// 	1,
		// 	100,
		// 	100,
		// 	100,
		// 	100,
		// 	100,
		// 	100,
		// 	1,
		// 	100,
		// 	100,
		// 	100,
		// 	100,
		// 	101,
		// 	100,
		// 	1,
		// 	100,
		// 	101,
		// 	100,
		// 	101,
		// };

		// glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 3, 8, 0, GL_RED, GL_UNSIGNED_BYTE, data.data());
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, character_width, character_height, 0, GL_RED, GL_UNSIGNED_BYTE, data.data());
	}

	void create_projectile_texture(GLuint &texture)
	{
		if (texture_map.count("projectile"))
		{
			texture = texture_map["projectile"];
			return;
		}
		// Create a texture for the character
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);

		// Set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		std::array<uint8_t, 16> data = {
			0, 7, 7, 0,
			7, 0, 0, 7,
			7, 0, 0, 7,
			0, 7, 7, 0};
		// std::array<uint8_t, 16> data = {
		// 	7,7,7,7
		// };

		glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, 4, 4, 0, GL_RED, GL_UNSIGNED_BYTE, data.data());
		texture_map["projectile"] = texture;
	}

	
	GLuint create_texture_from_data(std::string texture_name, const uint8_t *data, int width, int height)
	{
		if(texture_map.count(texture_name))
		{
			return texture_map[texture_name];
		}
		GLuint texture;
		// Create a texture for the character
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);

		// Set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		// std::array<uint8_t, 16> data = {
		// 	0, 7, 7, 0,
		// 	7, 0, 0, 7,
		// 	7, 0, 0, 7,
		// 	0, 7, 7, 0};
		// std::array<uint8_t, 16> data = {
		// 	7,7,7,7
		// };

		glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, data);
		texture_map[texture_name] = texture;
		return texture;
	}
}

std::vector<GLuint> load_shaders(std::string vert, std::string frag)
{
	std::vector<GLuint> programs;
	uint16_t shader_number = 1;

	// programs.push_back(glsl_helper::load_shader(glsl_helper::vert_0.str(), GL_VERTEX_SHADER));
	for (int i = 0; i < shader_number; i++)
	{
		GLuint vert_shader = glsl_helper::load_shader(vert, GL_VERTEX_SHADER);
		GLuint frag_shader = glsl_helper::load_shader(frag, GL_FRAGMENT_SHADER);

		GLuint program = glCreateProgram();
		if (program == 0)
		{
			std::printf("Failed to create shader program %d with error %d\n", i, glGetError());
		}
		glAttachShader(program, vert_shader);
		printf("Error_0: %d\n", glGetError());
		glAttachShader(program, frag_shader);
		printf("Error_1: %d\n", glGetError());
		glLinkProgram(program);
		printf("Error_2: %d\n", glGetError());
		programs.push_back(program);

		GLint linkStatus;
		glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
		if (linkStatus == GL_FALSE)
		{
			printf("Failed to link shader program\n");
			GLint logLength;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
			char *log = (char *)malloc(logLength);
			glGetProgramInfoLog(program, logLength, NULL, log);
			printf("Log: %s\n", log);
			free(log);
			// return -1;
		}
		else
		{
			printf("Shader program %d linked\n", i);
		}

		glDeleteShader(vert_shader);
		glDeleteShader(frag_shader);

		if (glGetError() != GL_NO_ERROR)
		{
			std::printf("Failed to load shader %d with error %d\n", i, glGetError());
		}
		// programs.push_back(glsl_helper::load_shader(glsl_helper::vert_0(), GL_VERTEX_SHADER));
		// programs.push_back(glsl_helper::load_shader(glsl_helper::frag_0(), GL_FRAGMENT_SHADER));
	}
	return programs;
}

GLuint load_compute_shader(std::string shader_src)
{
	printf("load_compute_shader 0: %d\n", glGetError());

	GLuint compute_shader = glsl_helper::load_shader(shader_src, GL_COMPUTE_SHADER);
	printf("load_compute_shader 1: %d\n", glGetError());
	GLuint program = glCreateProgram();
	if (program == 0)
	{
		std::printf("Failed to create shader program with error %d\n", glGetError());
	}
	printf("load_compute_shader 2: %d\n", glGetError());

	glAttachShader(program, compute_shader);
	printf("load_compute_shader 3: %d\n", glGetError());
	glLinkProgram(program);
	printf("load_compute_shader 4: %d\n", glGetError());

	GLint linkStatus;
	glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
	if (linkStatus == GL_FALSE)
	{
		printf("Failed to link shader program\n");
		GLint logLength;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
		char *log = (char *)malloc(logLength);
		glGetProgramInfoLog(program, logLength, NULL, log);
		printf("Error log: %s\n", log);
		free(log);
		// return -1;
	}
	else
	{
		printf("Shader program linked\n");
	}

	glDeleteShader(compute_shader);

	return program;
}

