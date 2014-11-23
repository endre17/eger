//OpenCV Headers
#include<cv.h>
#include<highgui.h>
//Input-Output
#include<stdio.h>
//Blob Library Headers
#include<cvblob.h>
//Kamerakep magassaga, szelessege
#define h 240
#define w 320
//Ciklusok szama, ameddig adott szinnek szerepelnie kell, hogy egerrel elvegezzuk a muveletet
#define sure 30

//X11 lib headers egermuveletekhez.
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/extensions/XTest.h>

//NameSpaces
using namespace cvb;
using namespace std;

int blue_count=0, green_count=0;
int minarea=60, maxarea=2000;
int sure_blue1=0, sure_blue2=0, sure_blue3=0;

//előző helyzete a zöldnek
double prev = 0;
//Ket piros tartomany is van de egyiket tudom csak beadni.
int lower_red[3] = {165, 80, 80};
int upper_red[3] = {180, 255, 255};
//int lower_red[3] = {0, 180, 180};
//int upper_red[3] = {10, 255, 255};
int lower_blue[3] = {95, 135, 135};
int upper_blue[3] = {125, 255, 255};
int lower_green[3] = {35, 80, 80};
int upper_green[3] = {75, 255, 255};

void detect_blob(IplImage* hsvframe, int* lower, int* upper, IplImage* threshold, IplImage* labelImg, CvBlobs& blobs, IplImage* frame){

    //inputkepbol(hsvframe) a megadott intervallumu szineket hagyva meg teszi az outputkepbe(threshold)
    cvInRangeS(hsvframe,cvScalar(lower[0],lower[1],lower[2]),cvScalar(upper[0],upper[1],upper[2]),threshold);
    //A threshold kep szurese
    cvSmooth(threshold,threshold,CV_MEDIAN,7,7);
    //A blob-ok megkeresese. (result=pixelek szama)
    unsigned int result=cvLabel(threshold,labelImg,blobs);
    //Blobok hozzaadasa kephez
    cvRenderBlobs(labelImg,blobs,frame,frame);
    //Adott meretnek kisebb, ill. nagyobb teruletet figyelmen kivul hagyjuk
    cvFilterByArea(blobs,minarea,maxarea);

}

void move_cursor(Display* display, int x, int y){

    XTestFakeMotionEvent(display, -1, x, y, CurrentTime);
    XSync(display, 0);

}


void mouse_click(Display* display, int button_nmbr){

    XTestFakeButtonEvent (display, button_nmbr, True,  CurrentTime);
    XTestFakeButtonEvent (display, button_nmbr, False,  CurrentTime);
    XSync(display, 0);

}

void double_click(Display* display, int button_nmbr){

    XTestFakeButtonEvent (display, button_nmbr, True,  CurrentTime);
    XTestFakeButtonEvent (display, button_nmbr, False,  CurrentTime);
    XTestFakeButtonEvent (display, button_nmbr, True,  CurrentTime);
    XTestFakeButtonEvent (display, button_nmbr, False,  CurrentTime);
    XSync(display, 0);

}

int main()
{

        Display *dpy = XOpenDisplay(NULL);
        Screen*  scrn = DefaultScreenOfDisplay(dpy);

        //Structure to get feed from CAM
        CvCapture* capture=cvCreateCameraCapture(0);

        //Structure to hold blobs
        CvBlobs red_blobs, blue_blobs, green_blobs;
        //Windows
        cvNamedWindow("Live",CV_WINDOW_AUTOSIZE);
        //Image Variables
        IplImage *frame=cvCreateImage(cvSize(w,h),8,3);   //Original Image
        IplImage *hsvframe=cvCreateImage(cvSize(w,h),8,3);//Image in HSV color space
        IplImage *labelImg_red=cvCreateImage(cvSize(w,h),IPL_DEPTH_LABEL,1);//Image Variable for blobs
        IplImage *labelImg_blue=cvCreateImage(cvSize(w,h),IPL_DEPTH_LABEL,1);//Image Variable for blobs
        IplImage *labelImg_green=cvCreateImage(cvSize(w,h),IPL_DEPTH_LABEL,1);//Image Variable for blobs
        //IplImage *threshy=cvCreateImage(cvSize(w,h),8,1); //Threshold image of yellow color
        IplImage *threshr=cvCreateImage(cvSize(w,h),8,1); //Threshold image of red color
        IplImage *threshb=cvCreateImage(cvSize(w,h),8,1); //Threshold image of blue color
        IplImage *threshg=cvCreateImage(cvSize(w,h),8,1); //Threshold image of green color

        //Getting the screen information

        int screenx = scrn->width;
        int screeny = scrn->height;

        if (!capture) {
            cerr<< "Error initializing capture" << endl;
            exit(1);
        }

        while(1)
        {
                blue_count=0;
                green_count=0;

                //Getting the current frame
                IplImage *fram=cvQueryFrame(capture);
                //If failed to get break the loop
                if(!fram)
                break;
                //Resizing the capture
                cvResize(fram,frame,CV_INTER_LINEAR );
                //Flipping the frame
                cvFlip(frame,frame,1);
                //Changing the color space
                cvCvtColor(frame,hsvframe,CV_BGR2HSV);

                //Piros teruletek keresese
                detect_blob(hsvframe, lower_red, upper_red, threshr, labelImg_red, red_blobs, frame);

                //Kek teruletek keresese
                detect_blob(hsvframe, lower_blue, upper_blue, threshb, labelImg_blue, blue_blobs, frame);

                //Zold terulek keresese
                detect_blob(hsvframe, lower_green, upper_green, threshg, labelImg_green, green_blobs, frame);


                //Koordinatakat adja meg a blobhoz(kurzor mozgatashoz jo)
                for (CvBlobs::const_iterator it=red_blobs.begin(); it!=red_blobs.end(); ++it)
                {

                        double a = it->second->centroid.x;
                        double b = it->second->centroid.y;

                        //Ha a kamera is arra 'nez', amerre mi akkor kell ez
                        //int x=screenx-((int)(a*screenx/w));
                        //int y=(int)(b*screeny/h);

                        //Ha a kamera szemben van, akkor kell ez
                        int x=(int)(a*screenx/w);
                        int y=screeny-((int)(b*screeny/h));

                        //Egerkuzor mozgatasa

                        move_cursor(dpy, x, y);
                        break;
                }


                for (CvBlobs::const_iterator it=green_blobs.begin(); it!=green_blobs.end(); ++it)
                {

                        double next = it->second->centroid.y;

                         //Ha a kamera is arra 'nez', amerre mi akkor kell ez
                         /*
                        if(next - prev > 0){
                            mouse_click(dpy, 5);
                        }else{
                            mouse_click(dpy, 4);
                        }
                        */
                        //Ha a kamera szemben van, akkor kell ez
                        if(next - prev > 0){
                            mouse_click(dpy, 4);
                        }else{
                            mouse_click(dpy, 5);
                        }

                        //cout<<"Pozicio:"<<next<<endl;
                        prev = next;
                        break;
                }


                //Kek teruletekbol mennyi van adott kepkockan.
                for (CvBlobs::const_iterator it=blue_blobs.begin(); it!=blue_blobs.end(); ++it){
                    blue_count++;
                }
                /*
                if(blue_count){
                    cout<<"Kek teruletek: "<<blue_count<<endl;
                }
                */


                //Ahhoz, hogy tudjuk biztosra, hogy milyen kattintast akarunk vegezni noveljunk az ahhoz adott tartozo valtozot aszerint hogy mennyi olyan szinu terulet van a kepen.
                if(blue_count==1){
                    sure_blue1++;
                    sure_blue2=0;
                    sure_blue3=0;
                }else if(blue_count==2){
                    sure_blue2++;
                    sure_blue1=0;
                    sure_blue3=0;
                }else if(blue_count==3){
                    sure_blue3++;
                    sure_blue1=0;
                    sure_blue2=0;
                }else{
                    sure_blue1=0;
                    sure_blue2=0;
                    sure_blue3=0;
                }



                //cout<<"Biztos 1 kek: "<<sure_blue1<<endl;
                //cout<<"Biztos 2 kek: "<<sure_blue2<<endl;
                //cout<<"Biztos 3 kek: "<<sure_blue3<<endl;

                //Ha valamelyik eleri a biztos erteket, akkor vegrehajtuk a muveletet, es visszaallitjuk a kezdoerteket.
                if(sure_blue1==sure){
                    //Bal kattintas
                    mouse_click(dpy, 1);
                    sure_blue1=0;

                }else if(sure_blue2==sure){
                    //Dupla bal kattintas
                    double_click(dpy, 1);
                    sure_blue2=0;
                }else if(sure_blue3==sure){
                    //Jobb kattintas
                    mouse_click(dpy, 3);
                    sure_blue3=0;

                }

                //Showing the images
                cvShowImage("Live",frame);
                //Escape Sequence
                char c=cvWaitKey(33);
                if(c==27)
                break;
        }
        //Cleanup
        XCloseDisplay(dpy);
        cvReleaseCapture(&capture);
        cvDestroyAllWindows();

}
