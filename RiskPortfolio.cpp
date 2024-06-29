// RiskPortfolio.cpp

#include "RiskPortfolio.h"
#include <iostream>

RiskPortfolio::RiskPortfolio(const Eigen::VectorXd& dv01, const Eigen::VectorXd& vega, const Eigen::VectorXd& price, double capitalLimit)
    : DV01(dv01), Vega(vega), Price(price), CapitalLimit(capitalLimit) {}

Eigen::VectorXd RiskPortfolio::createRiskPortfolio() {
    int n = DV01.size();

    // Formulate the optimization problem
    Eigen::MatrixXd A(3, n);
    A.row(0) = DV01.transpose();
    A.row(1) = Vega.transpose();
    A.row(2) = Price.transpose();

    Eigen::VectorXd b(3);
    b << 0, 0, CapitalLimit;

    std::cout << "Matrix A:\n" << A << std::endl;
    std::cout << "Vector b:\n" << b << std::endl;

    // Solve the linear system Ax = b for the trade weights
    Eigen::VectorXd weights = A.fullPivLu().solve(b);


    // Verify the solution by calculating A * weights and comparing with b
    Eigen::VectorXd verification = A * weights;
    std::cout << "Verification (A * weights):\n" << verification << std::endl;
    std::cout << "Expected (b):\n" << b << std::endl;

    // Check the rank of A
    Eigen::FullPivLU<Eigen::MatrixXd> lu_decomp(A);
    int rank = lu_decomp.rank();
    std::cout << "Rank of matrix A: " << rank << std::endl;

    return weights;
}
