'''
Macro which takes a CSV of cylinders and plots them in 3D so they can be visualised. 
'''


import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

def data_for_cylinder_along_z(center_x,center_y,radius,minZ,maxZ):
    z = np.linspace(minZ, maxZ, 50)

    
    theta = np.linspace(0, 2*np.pi, 50)
    theta_grid, z_grid=np.meshgrid(theta, z)
    x_grid = radius*np.cos(theta_grid) + center_x
    y_grid = radius*np.sin(theta_grid) + center_y
    return x_grid,y_grid,z_grid





def main(cylinder_filepath,savefig_loc):

    df = pd.read_csv(cylinder_filepath)

    Zmin = df.min()['Zmin']
    Zmax = df.max()['Zmax']

    fig = plt.figure()
    ax = fig.add_subplot(111, projection='3d')

    for index, row in df.iterrows():
        Zmin_row = row['Zmin']
        Zmax_row = row['Zmax']
        radius = row['radius']

        Xc,Yc,Zc = data_for_cylinder_along_z(0,0,radius,Zmin_row,Zmax_row)
        ax.plot_surface(Xc, Yc, Zc, alpha=0.5)
    

    plt.show()
    #plt.savefig("")

savefig_loc = "image_of_cylinders.png"
cylinder_filepath = "/home/answain/alice/CircularCrossSections/BestOptimisations/IncZDC5%HyperFine/cylinder_data.csv"
main(cylinder_filepath,savefig_loc)