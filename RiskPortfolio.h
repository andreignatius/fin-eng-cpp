// RiskPortfolio.h

#ifndef RISK_PORTFOLIO_H
#define RISK_PORTFOLIO_H

#include <Eigen/Dense>
#include <vector>

class RiskPortfolio {
public:
    RiskPortfolio(const Eigen::VectorXd& dv01, const Eigen::VectorXd& vega, const Eigen::VectorXd& price, double capitalLimit);
    Eigen::VectorXd createRiskPortfolio();

private:
    Eigen::VectorXd DV01;
    Eigen::VectorXd Vega;
    Eigen::VectorXd Price;
    double CapitalLimit;
};

#endif // RISK_PORTFOLIO_H
