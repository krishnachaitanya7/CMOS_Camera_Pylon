import numpy as np

if __name__ == "__main__":
    data_type = np.dtype(np.uint16).newbyteorder('>')
    cmos_array = np.fromfile("bin_files/P1_E2_T2__marks.bin", dtype=data_type)
    print(cmos_array)