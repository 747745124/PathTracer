#pragma once
#include <vector>
#include <numeric>
#include <cmath>
#include <algorithm> // For std::max
#include <iostream>  // For potential error streaming
#include "utils/utility.hpp"
// Structure to hold the results of the normalization test
struct PDFNormalizationTestResult
{
    double integral_estimate;
    int negative_pdf_count;
    int num_samples;
    bool passed_negative_check;
    bool passed_integral_check;
    double tolerance;
};

/**
 * @brief Tests the normalization of a PDF defined over 3D directions on a unit sphere.
 *
 * The PDF is expected to integrate to 1.0 over the unit sphere. If the PDF is
 * non-zero only on a hemisphere (e.g., for reflection) and returns 0 for the
 * other hemisphere, this test will still correctly estimate its integral over
 * the effective domain (the hemisphere) provided it's normalized to 1 there.
 *
 * @tparam PDFEvaluator A callable type (e.g., lambda, functor) that takes a
 * const gl::vec3& (a direction vector) and returns a float (the PDF value).
 * @param evaluate_pdf_at The callable PDF evaluator.
 * @param num_samples The number of Monte Carlo samples to use.
 * @param expected_integral The expected value of the integral (usually 1.0).
 * @param tolerance The allowed absolute difference from the expected_integral.
 * @return PDFNormalizationTestResult struct containing the test results.
 */
template <typename PDFEvaluator>
PDFNormalizationTestResult test_directional_pdf_normalization(
    PDFEvaluator evaluate_pdf_at,
    int num_samples = 200000,
    double expected_integral = 1.0,
    double tolerance = 0.02)
{
    double sum_pdf_values = 0.0;
    int negative_count = 0;

    for (int i = 0; i < num_samples; ++i)
    {
        // Generate two uniform random numbers for sphere sampling
        float u1 = gl::rand_num();
        float u2 = gl::rand_num();

        // Sample a direction uniformly on the unit sphere
        gl::vec3 sample_direction = gl::uniformSampleSphere(u1, u2);

        // Evaluate the PDF at the sampled direction
        float pdf_value = evaluate_pdf_at(sample_direction);

        if (pdf_value < 0.0f)
        {
            negative_count++;
            // For integral estimation, it's often best to clamp negative values
            // to 0 if they are considered errors, so they don't incorrectly
            // cancel out positive values if the integral check is separate.
            pdf_value = 0.0f;
        }
        sum_pdf_values += static_cast<double>(pdf_value);
    }

    double average_pdf_value = sum_pdf_values / static_cast<double>(num_samples);

    // The Monte Carlo estimate of the integral of the PDF over the unit sphere (area 4*PI)
    // E[pdf(X_uniform)] = (1 / Measure) * integral pdf(x) dx
    // Integral pdf(x) dx = Measure * E[pdf(X_uniform)]
    // Measure for unit sphere = 4 * PI
    double integral_estimate = average_pdf_value * (4.0 * M_PI);

    PDFNormalizationTestResult result;
    result.integral_estimate = integral_estimate;
    result.negative_pdf_count = negative_count;
    result.num_samples = num_samples;
    result.tolerance = tolerance;
    result.passed_negative_check = (negative_count == 0);
    result.passed_integral_check = (std::abs(integral_estimate - expected_integral) <= tolerance);

    return result;
}