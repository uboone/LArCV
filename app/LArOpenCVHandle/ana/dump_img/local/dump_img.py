import os,sys

if len(sys.argv) != 8: 
    print 
    print "WIRE_FILE   = str(sys.argv[1])"
    print "PGRAPH_FILE = str(sys.argv[2])"
    print "ENTRY       = int(sys.argv[3])"
    print "VTXID       = int(sys.argv[4])"
    print "PGRAPH_PROD = str(sys.argv[5])"
    print "PIXEL_PROD  = str(sys.argv[6])"
    print "OUT_DIR     = str(sys.argv[7])"
    print
    sys.exit(1)
    
WIRE_FILE   = str(sys.argv[1])
PGRAPH_FILE = str(sys.argv[2])
ENTRY       = int(sys.argv[3])
VTXID       = int(sys.argv[4])
PGRAPH_PROD = str(sys.argv[5])
PIXEL_PROD  = str(sys.argv[6])
OUT_DIR     = str(sys.argv[7])

from larcv import larcv
import numpy as np

import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

import ROOT
from ROOT import geo2d
pygeo = geo2d.PyDraw()

iom = larcv.IOManager()
iom.set_verbosity(2)
iom.add_in_file(WIRE_FILE)
iom.add_in_file(PGRAPH_FILE)
iom.initialize()
iom.read_entry(ENTRY)

ev_img    = iom.get_data(larcv.kProductImage2D,"wire")
ev_pgraph = iom.get_data(larcv.kProductPGraph,PGRAPH_PROD)
ev_ctor   = iom.get_data(larcv.kProductPixel2D,PIXEL_PROD)

print "@run=",ev_img.run(),"subrun=",ev_img.subrun(),"event=",ev_img.event()

print "GOT:",ev_img.Image2DArray().size(),"images"
print "GOT:",ev_pgraph.PGraphArray().size(),"vertices"
print "GOT:",ev_ctor.Pixel2DClusterArray().size(),"particle clusters"

pgraph = ev_pgraph.PGraphArray().at(VTXID)
parray = pgraph.ParticleArray()

print larcv.load_cvutil()
img_v = [ev_img.at(i).crop(parray.front().BB(i)) for i in xrange(3)]

lim = larcv.LArbysImageMaker()
lim._charge_max = 255
lim._charge_min = 0
lim._charge_to_gray_scale = 1

img_v = [lim.ExtractMat(img) for img in img_v]
img_v = [pygeo.image(img) for img in img_v]
img_v = [img.T[::-1,:].copy() for img in img_v]

X = pgraph.ParticleArray().front().X()
Y = pgraph.ParticleArray().front().Y()
Z = pgraph.ParticleArray().front().Z()

meta_v = pgraph.ParticleArray().front().BB()

colors=['magenta','white','yellow']
ctor_v = []

for parid,id_ in enumerate(np.array(pgraph.ClusterIndexArray())):
    pix2d_vv=[None,None,None]
    print "@parid=",parid,"&id_=",id_
    
    for plane in xrange(3):
        pix2d_v = ev_ctor.Pixel2DClusterArray(plane).at(id_)
        pix2d_v = [[pix2d_v[ii].X(),pix2d_v[ii].Y()] for ii in xrange(pix2d_v.size())]
        
        if len(pix2d_v)!=0: pix2d_v.append(pix2d_v[0])
            
        pix2d_vv[plane] = np.array(pix2d_v)

    ctor_v.append(pix2d_vv)
        
for plane in xrange(3):

    xpixel = ROOT.Double()
    ypixel = ROOT.Double()

    SS="Plane {}".format(plane)
    fig,ax=plt.subplots(figsize=(20,22))
    ax.imshow(img_v[plane],cmap='jet',vmin=0,vmax=255,interpolation='none')

    larcv.Project3D(meta_v[plane],X,Y,Z,0.0,plane,xpixel,ypixel)

    ax.plot(xpixel,meta_v[plane].rows()-ypixel,'*',color='yellow',markersize=30)

    xmin =  1e9
    xmax = -1e9

    ymin =  1e9
    ymax = -1e9

    for ix,ctor in enumerate(ctor_v):

        ctor=ctor[plane]
        if ctor.size==0: continue
        ax.plot(ctor[:,0],ctor[:,1],'-',lw=5,color=colors[ix],alpha=1.0)
        
        if ctor[:,0].min() < xmin : xmin = ctor[:,0].min()
        if ctor[:,0].max() > xmax : xmax = ctor[:,0].max()

        if ctor[:,1].min() < ymin : ymin = ctor[:,1].min()
        if ctor[:,1].max() > ymax : ymax = ctor[:,1].max()

    
    if xmin != 1e9:
        ax.set_xlim(xmin-25,xmax+25)
        ax.set_ylim(ymin-25,ymax+25)

    SS = "{}_{}_{} Plane={}".format(ev_img.run(),ev_img.subrun(),ev_img.event(),plane)
    ax.set_title(SS,fontweight='bold',fontsize=50)        

    this_num = os.path.basename(WIRE_FILE).split(".")[0].split("_")[-1]
    SS=os.path.join(OUT_DIR,"{}_{}_{}_{}_{}_{}_{}_{}.png".format(ev_img.run(),
                                                                 ev_img.subrun(),
                                                                 ev_img.event(),
                                                                 this_num,
                                                                 ENTRY,
                                                                 VTXID,
                                                                 parid,
                                                                 plane))
    
    
    plt.savefig(SS)
    plt.clf()
    plt.cla()
    plt.close()

        
sys.exit(1)
