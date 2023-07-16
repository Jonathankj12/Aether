epsilon_infty(0)

name("ITO - 400nm to 800nm - Tabulated")

lambda={4.00000000e-007,4.48000000e-007,4.52000000e-007,5.00000000e-007,5.52000000e-007,6.00000000e-007,6.52000000e-007,7.00000000e-007,7.52000000e-007,8.00000000e-007}
nr={2.30000000e+000,2.12720000e+000,2.11680000e+000,2.04000000e+000,1.99840000e+000,1.96000000e+000,1.92920000e+000,1.91000000e+000,1.87880000e+000,1.85000000e+000}
ni={2.00000000e-002,1.04000000e-002,1.00000000e-002,1.00000000e-002,1.00000000e-002,1.00000000e-002,1.00000000e-002,1.00000000e-002,1.00000000e-002,1.00000000e-002}

add_data_index(lambda,nr,ni)

validity_range(400e-9,800e-9)