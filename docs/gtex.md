# GTex (GCTk Texture format)

## Header

| Name                    | Size        | Typical value                | Description                                                                                           |
|-------------------------|-------------|------------------------------|-------------------------------------------------------------------------------------------------------|
| Identifier              | 4 bytes     | "GTEX"                       | 4 ASCII characters used for identifying the file, spelling out "GTEX"                                 |
| Version                 | 1 byte      | 0                            | Current texture format version. Not implemented yet, currently ignored.                               |
| Flags                   | 1 byte      | See: [Flags](#flags)         | A bit field used for texture properties.                                                              |
| Pixel format            | 1 byte      | See: [Pixel format](#format) | Pixel format that the image data uses.                                                                |
| Width                   | 2 bytes     | More then 0                  | Texture's width                                                                                       |
| Height (optional)       | 2 bytes     | More then 0                  | Texture's height, only used for 2D, 3D and Cubemap textures                                           |
| Depth (optional)        | 2 bytes     | More then 0                  | Texture's depth, only used for 3D textures                                                            |
| Palette size (optional) | 1 - 2 bytes | More then 0                  | Count of colours defined in the colour palette. 1 byte long if INDEXED_8, 2 bytes long if INDEXED_16. |

<h3 name="flags">Flags</h3>

| Name             | Size   | Description                                                         |
|------------------|--------|---------------------------------------------------------------------|
| Texture Target   | 3 bits | Texture target to use. For more info, see [Texture Target](#target) |
| Clamp R          | 1 bit  | Clamps texture along the Z axis                                     |
| Clamp S          | 1 bit  | Clamps texture along the Y axis                                     |
| Clamp T          | 1 bit  | Clamps texture along the X axis                                     |
| Point filter     | 1 bit  | Use "Nearest" filter instead of "Linear", keeps sharp edges.        |
| Generate mipmaps | 1 bit  | Generate mipmaps if set to '1'                                      |

<h3 name="target">Texture Target</h3>

| Name           | Value | Description                                            |
|----------------|-------|--------------------------------------------------------|
| 1D             | 0     | 1D texture, used for simple gradients.                 |
| 2D             | 1     | 2D texture, most commonly used target.                 |
| 3D             | 2     | 3D texture, currently not used.                        |
| Cubemap        | 3     | Cubemap texture, used by skyboxes and reflections.     |
| 1D Array       | 4     | Array of 1D textures.                                  |
| 2D Array       | 5     | Array of 2D textures, used for animation and tilemaps. |
| Cubemap Array  | 6     | Array of cubemaps.                                     |

<h3 name="format">Pixel format</h3>

| Name                  | Value | Pixel size      | Description                                                                                          |
|-----------------------|-------|-----------------|------------------------------------------------------------------------------------------------------|
| Grayscale             | 0     | 1 byte / pixel  | Grayscale texture (R)                                                                                |
| Grayscale with alpha  | 1     | 2 bytes / pixel | Grayscale texture with alpha (RG)                                                                    |
| Indexed 8             | 2     | 1 byte / pixel  | Indexed format using 8bit index, uses RGB after unpacked.                                            |
| Indexed 8 with alpha  | 3     | 2 bytes / pixel | Indexed format using 8bit index, with 8bit alpha, uses RGBA after unpacked.                          |
| Indexed 16            | 4     | 2 byte / pixel  | Indexed format using 16bit index, uses RGB after unpacked.                                           |
| Indexed 16 with alpha | 5     | 3 bytes / pixel | Indexed format using 16bit index, with 8bit alpha, uses RGBA after unpacked.                         |
| RGB                   | 6     | 3 bytes / pixel | Full colour 24bit texture made out of 3 components (1 byte per component) (RGB)                      |
| RGBA                  | 7     | 3 bytes / pixel | Full colour 32bit texture made out of 4 components (1 byte per component) with alpha channel. (RGBA) |
