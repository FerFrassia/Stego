#include <math.h>
#include <vector>
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics.hpp>
#include "Metrics.h"

using namespace std;

/*!
@method determine_mean_squared_error between 2 images
@abstract estimate the average of the squares of the errors of the chars in the images.
Returns MSE.
@params input_img_path: The name of the image to compare.
@params input_img_path_2: The name of the second image to compare.
*/
double Metrics::determine_mean_squared_error(
  BMPImage &input_img,
  BMPImage &input_img_2
) {
    double img_size = (double) input_img.get_image_size();
    double mean_squared_error = 0;

    for (size_t i = 0; i < img_size; i++){
        bmp_pixel pixel = input_img.read_pixel();
        bmp_pixel pixel_2 = input_img_2.read_pixel();

        double blue_difference = ((double) pixel.blue - pixel_2.blue);
        blue_difference *= blue_difference;
        double green_difference = ((double) pixel.green - pixel_2.green);
        green_difference *= green_difference;
        double red_difference = ((double) pixel.red - pixel_2.red);
        red_difference *= red_difference;
        mean_squared_error += blue_difference + green_difference + red_difference;

        input_img.next_pixel();
        input_img_2.next_pixel();
    }
    
    mean_squared_error /= img_size*3;
    return mean_squared_error;
}


/*!
@method determine_PSNR between 2 images
@abstract estimate PSNR of the images images.
Returns MSE.
@params input_img_path: The name of the image to compare.
@params input_img_path_2: The name of the second image to compare.
 */
double Metrics::determine_PSNR(
  BMPImage &input_img,
  BMPImage &input_img_2
) {
    double mse = determine_mean_squared_error(input_img, input_img_2);
    double peak_signalToNoise_ratio = 20 * log10(255) - 10 * log10(mse);
    return peak_signalToNoise_ratio;
}

double mean(vector<double> arr, int n) {
    double sum = 0;
    for(int i = 0; i < n; i++)
        sum += arr[i];
    return sum / n;
}

double variance(vector<double> arr, int n) {
    double var = 0;
    double mu = mean(arr, n);
    for (int i = 0; i < n; i++) {
        var += (arr[i] - mu) * (arr[i] - mu);
    }
    var /= n;
    return var;
}

double covariance(vector<double> arr1, vector<double> arr2, int n)
{
    double sum = 0;
    double mu_x = mean(arr1, n);
    double mu_y = mean(arr2, n);

    for(int i = 0; i < n; i++)
        sum += (arr1[i] - mu_x) *
               (arr2[i] - mu_y);
    return sum / (n - 1);
}

/*!
@method determine_SSIM between 2 images
@abstract estimate SSIM of the images images.
Returns SSIM.
@params input_img_path: The name of the image to compare.
@params input_img_path_2: The name of the second image to compare.
 */
double Metrics::determine_SSIM(
  BMPImage &input_img,
  BMPImage &input_img_2
) {

    int L = 255;
    double c1 = pow((0.01 * L), 2);
    double c2 = pow((0.03 * L), 2);

    double img_size = (double) input_img.get_image_size();
    int N = img_size*3;

    vector<double> pixels1(N), pixels2(N);

    for (size_t i = 0; i < img_size; i++){
        bmp_pixel pixel = input_img.read_pixel();
        bmp_pixel pixel_2 = input_img_2.read_pixel();
        pixels1[i * 3] = pixel.red;
        pixels1[i * 3 + 1] = pixel.green;
        pixels1[i * 3 + 2] = pixel.blue;
        pixels2[i * 3] = pixel_2.red;
        pixels2[i * 3 + 1] = pixel_2.green;
        pixels2[i * 3 + 2] = pixel_2.blue;

        input_img.next_pixel();
        input_img_2.next_pixel();
    }

    double mu_x = mean(pixels1, N);
    double mu_y = mean(pixels2, N);
    double sigma_x = variance(pixels1, N);
    double sigma_y = variance(pixels2, N);
    double sigma_xy = covariance(pixels1, pixels2, N);

    return (2 * mu_x * mu_y + c1)*(2 * sigma_xy + c2) /
            ((mu_x * mu_x + mu_y * mu_y + c1) * (sigma_x + sigma_y + c2));
}
