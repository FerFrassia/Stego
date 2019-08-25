#include <math.h>
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics.hpp>
#include "Metrics.h"

/*!
@method determine_mean_squared_error between 2 images
@abstract estimate the average of the squares of the errors of the chars in the images.
Returns MSE.
@params input_img_path: The name of the image to compare.
@params input_img_path_2: The name of the second image to compare.
@params number_of_chars: The amount of chars to check as source.
*/
double Metrics::determine_mean_squared_error(
  BMPImage &input_img,
  BMPImage &input_img_2,
  unsigned int number_of_chars
) {
    double d_chars  = (double) number_of_chars;
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
@params number_of_chars: The amount of chars to check as source.
 */
double Metrics::determine_PSNR(
  BMPImage &input_img,
  BMPImage &input_img_2,
  unsigned int number_of_chars
) {
    double mse = determine_mean_squared_error(input_img, input_img_2, number_of_chars);
    double peak_signalToNoise_ratio = 20 * log10(255) - 10 * log10(mse);
    return peak_signalToNoise_ratio;
}

/*!
@method determine_SSIM between 2 images
@abstract estimate SSIM of the images images.
Returns SSIM.
@params input_img_path: The name of the image to compare.
@params input_img_path_2: The name of the second image to compare.
@params number_of_chars: The amount of chars to check as source.
 */
double Metrics::determine_SSIM(
  BMPImage &input_img,
  BMPImage &input_img_2,
  unsigned int number_of_chars
) {

    int L = 255;
    double c1 = pow((0.01 * L), 2);
    double c2 = pow((0.03 * L), 2);

    double d_chars  = (double) number_of_chars;
    double img_size = (double) input_img.get_image_size();
    double average_x = 0;
    double average_y = 0;

    // std::vector<vector<> > datos (img_size*3,0);

    // accumulator_set<double, stats<tag::variance> > acc;
    // for_each(datos.begin(), datos.end(), bind<void>(ref(acc), _1));




    cout << mean(acc) << endl;
    cout << sqrt(variance(acc)) << endl;

    double mean = mean(acc);
    double variance = sqrt(variance(acc));


    for (size_t i = 0; i < img_size; i++){
        bmp_pixel pixel = input_img.read_pixel();
        bmp_pixel pixel_2 = input_img_2.read_pixel();

        average_x += pixel.blue + pixel.green + pixel.red;
        average_y += pixel_2.blue + pixel_2.green + pixel_2.red;
        input_img.next_pixel();
        input_img_2.next_pixel();
    }

}
