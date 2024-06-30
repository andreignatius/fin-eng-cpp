import numpy as np
import pandas as pd
from scipy.optimize import minimize

def parse_data(file_path):
    """Load data from CSV."""
    return pd.read_csv(file_path)

def minimize_risks(coefficients, lambda_reg=0.01):
    """Minimize the sum of squared risks using Tikhonov regularization."""
    A = coefficients.T  # Transpose to get risks as rows and assets as columns
    U, s, Vt = np.linalg.svd(A, full_matrices=False)
    s_reg = s / (s**2 + lambda_reg**2)  # Regularize singular values
    pinv_reg = Vt.T @ np.diag(s_reg) @ U.T
    b = np.zeros(A.shape[0])  # Target zero risks, minimal risk instead of neutral
    weights = pinv_reg @ b

    # Minimize sum of squared risks
    def objective(w):
        return np.sum((A @ w)**2) + lambda_reg * np.sum(w**2)

    constraints = {'type': 'eq', 'fun': lambda w: np.sum(w) - 1}  # Sum of weights must be 1
    bounds = [(0, None) for _ in range(A.shape[1])]  # No short selling
    result = minimize(objective, weights, method='SLSQP', bounds=bounds, constraints=[constraints])

    if result.success:
        return result.x
    else:
        print("Optimization Diagnostic Info:", result)
        raise Exception("Optimization failed: " + result.message)


def maximize_pnL(coefficients, PnL, target_DV01=0, target_Vega=0, lambda_reg=0.1):
    """Maximize PnL under fixed risk limits of DV01 and Vega, with increased regularization."""
    num_assets = coefficients.shape[0]

    def objective(weights):
        regularization = lambda_reg * np.sum(weights**2)
        return -np.dot(PnL, weights) + regularization

    constraints = [
        {'type': 'eq', 'fun': lambda weights: np.dot(coefficients[:, 0], weights) - target_DV01},
        {'type': 'eq', 'fun': lambda weights: np.dot(coefficients[:, 1], weights) - target_Vega}
    ]

    bounds = [(None, None) for _ in range(num_assets)]
    initial_guess = np.zeros(num_assets)
    options = {'maxiter': 1000}

    result = minimize(objective, initial_guess, method='SLSQP', bounds=bounds, constraints=constraints, options=options)

    if not result.success:
        print("Optimization Diagnostic Info:", result)
        raise Exception("Optimization failed: " + result.message)
    return result.x

def analyze_risks(coefficients):
    rank = np.linalg.matrix_rank(coefficients)
    print("Rank of the coefficient matrix:", rank)
    if rank < coefficients.shape[0]:
        print("Dependent risk factors detected.")



# Example usage
file_path = 'portfolio_data_with_randoms.csv'
df = parse_data(file_path)
coefficients = df[['DV01', 'Vega']].values  # Get the coefficients for DV01 and Vega
PnL = df['PV'].values  # Present Value from the portfolio

neutral_weights = minimize_risks(coefficients)
print("Weights to minimize DV01 and Vega:", neutral_weights)

analyze_risks(coefficients)

optimal_weights = maximize_pnL(coefficients, PnL)
print("Optimal weights for maximal PnL under risk limits:", optimal_weights)
