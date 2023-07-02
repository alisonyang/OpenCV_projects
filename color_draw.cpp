#include "opencv2/opencv.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "iostream"
using namespace cv;
using namespace std;

Mat img;
vector<vector<int>> new_points;

// use color picker to get value: {hmin, smin, vmin, hmax, smax, vmax}
vector<vector<int>> obj_colors {
                                {165, 130, 84, 179, 196, 255}, // pink
                                {35, 27, 51, 58, 255, 255} // green
                              };
vector<Scalar> paint_colors {
                                {250, 0, 254}, // pink
                                {144, 238, 144} // green
                              };

Point getContours(const Mat& mask) {

    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;

    findContours(mask, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
//    drawContours(img, contours, -1, Scalar(255,0,255), 2 );

    vector<vector<Point>> conpoly(contours.size());
    vector<Rect> bound_rect(contours.size());

    Point obj_point(0, 0);
//    remove noise
    for (int i = 0; i < contours.size(); i++)
    {
        int area = (int) contourArea(contours[i]);
//        cout << area << endl;
        if (area > 1000)
        {
            double peri = arcLength(contours[i], true);
            approxPolyDP(contours[i], conpoly[i], 0.02*peri, true);
            bound_rect[i] = boundingRect(conpoly[i]);

            obj_point.x = bound_rect[i].x + bound_rect[i].width / 2;
            obj_point.y = bound_rect[i].y;

//            optional function
//            drawContours(img, conpoly, i, Scalar(255,0,255), 2 );
//            rectangle(img, bound_rect[i].tl(), bound_rect[i].br(), Scalar(0, 255,0), 5);
        }
    }
    return obj_point;
};
vector<vector<int>> findColor(Mat img)
{
    Mat imgHSV;
    cvtColor(img, imgHSV, COLOR_BGR2HSV);

    for (int i = 0; i < obj_colors.size(); i++)
    {
        Scalar lower(obj_colors[i][0], obj_colors[i][1], obj_colors[i][2]);
        Scalar upper(obj_colors[i][3], obj_colors[i][4], obj_colors[i][5]);
        Mat mask;
        inRange(imgHSV, lower, upper, mask);
//        imshow(to_string(i), mask);
        Point obj_point = getContours(mask);

        if (obj_point.x != 0 && obj_point.y != 0) {
            new_points.push_back({obj_point.x, obj_point.y, i});
        }

    }
    return new_points;
}

void drawOnCanvas(vector<vector<int>> new_points, vector<Scalar> paint_colors)
{
    for (int i = 0; i < new_points.size(); i++)
    {
        circle(img,Point(new_points[i][0],new_points[i][1]), 10, paint_colors[new_points[i][2]], FILLED);
    }
}

int main() {

    VideoCapture cap(1);


    while (true){
        cap.read(img);
        new_points = findColor(img);
        drawOnCanvas(new_points, paint_colors);
        imshow("Image", img);
        waitKey(1);
    }

}