from PIL import Image
import sys

def convert_bmp_to_escpos_raster(bmp_file, output_file):
    # Open the BMP file
    img = Image.open(bmp_file)
    # Convert to 1-bit black & white
    img = img.convert('1')

    # BMP image width and height
    width, height = img.size
    print(f"Image size: {width}x{height}")

    # Width in bytes (1 byte = 8 pixels)
    width_bytes = (width + 7) // 8

    # Prepare raster data
    raster_data = bytearray()

    # For each row
    for y in range(height):
        byte = 0
        bit_count = 0
        for x in range(width):
            pixel = img.getpixel((x, y))
            if pixel == 0:  # Black pixel
                byte |= (1 << (7 - bit_count))
            bit_count += 1
            if bit_count == 8:
                raster_data.append(byte)
                byte = 0
                bit_count = 0
        if bit_count > 0:
            raster_data.append(byte)

    # Save raster data to output file
    with open(output_file, 'wb') as f:
        f.write(raster_data)
    print(f"Raster data saved to {output_file}")

    # Print recommended command snippet for your C program
    print("\nC Snippet for GS v 0 command:")
    print(f"Width bytes: {width_bytes}")
    print(f"Height: {height}")
    # print("const char rasterHeader[] = { 0x1D, 'v', '0', 0, "
    #       f"{width_bytes & 0xFF}, {(width_bytes >> 8) & 0xFF}, "
    #       f"{height & 0xFF}, {(height >> 8) & 0xFF} };")

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python bmp2escpos.py input.bmp output.bin")
        sys.exit(1)

    bmp_file = sys.argv[1]
    output_file = sys.argv[2]

    convert_bmp_to_escpos_raster(bmp_file, output_file)