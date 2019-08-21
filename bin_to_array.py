import numpy as np

if __name__ == "__main__":
    data_type = np.dtype(np.uint16).newbyteorder('>')
    cmos_array = np.fromfile("bin_files/P1_E3_T2_left_video.bin", dtype=data_type)
    print("Non Zero Elements:", (cmos_array != 0).sum())
    print("Zero Elements:", (cmos_array == 0).sum())
    print("Max Element:", np.amax(cmos_array))
