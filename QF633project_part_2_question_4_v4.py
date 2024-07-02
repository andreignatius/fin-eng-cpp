import numpy as np
from scipy.optimize import minimize



# def optimize_portfolio(dv01_vectors, vega_vectors):
#     """
#     Optimizes the portfolio to minimize DV01 and vega deviations from zero.
#
#     Parameters:
#     dv01_vectors (np.ndarray): Matrix of DV01 vectors for tradable items.
#     vega_vectors (np.ndarray): Matrix of vega vectors for tradable items.
#
#     Returns:
#     np.ndarray: Optimal weights for the portfolio.
#     """
#
#     # Ensure input matrices are numpy arrays
#     dv01_vectors = np.array(dv01_vectors)
#     vega_vectors = np.array(vega_vectors)
#
#     # Number of tradable items
#     n_items = dv01_vectors.shape[0]
#
#     # Objective function to minimize
#     def objective(weights):
#         portfolio_dv01 = np.dot(weights, dv01_vectors)
#         portfolio_vega = np.dot(weights, vega_vectors)
#         # Minimize the sum of squares of DV01 and Vega
#         return np.sum(portfolio_dv01 ** 2) + np.sum(portfolio_vega ** 2)
#
#     # Constraints: sum of weights = 1 (for a fully invested portfolio)
#     constraints = ({
#         'type': 'eq',
#         'fun': lambda weights: np.sum(weights) - 1
#     })
#
#     # Bounds for the weights: assuming they can be between -1 and 1 for shorting
#     bounds = [(-1, 1) for _ in range(n_items)]
#
#     # Initial guess (equal weight for each item)
#     initial_guess = np.ones(n_items) / n_items
#
#     # Solve the optimization problem
#     result = minimize(objective, initial_guess, bounds=bounds, constraints=constraints)
#
#     # Check if the optimization was successful
#     if result.success:
#         optimal_weights = result.x
#     else:
#         raise ValueError("Optimization failed: " + result.message)
#
#     return optimal_weights






def optimize_portfolio_pnl(pnl, dv01_vectors, vega_vectors, dv01_min, dv01_max, vega_min, vega_max):
    # Objective function to maximize the total PnL
    def objective(x):
        return -np.dot(x, pnl)

    # Constraint functions
    def dv01_lower_bound(x):
        return np.dot(x, dv01_vectors).sum() - dv01_min

    def dv01_upper_bound(x):
        return dv01_max - np.dot(x, dv01_vectors).sum()

    def vega_lower_bound(x):
        return np.dot(x, vega_vectors).sum() - vega_min

    def vega_upper_bound(x):
        return vega_max - np.dot(x, vega_vectors).sum()

    # Initial guess
    x0 = np.ones(len(pnl)) / len(pnl)

    # Bounds for the weights
    bounds = [(0, 10) for _ in range(len(pnl))]

    # Constraints
    constraints = [
        {'type': 'ineq', 'fun': dv01_lower_bound},
        {'type': 'ineq', 'fun': dv01_upper_bound},
        {'type': 'ineq', 'fun': vega_lower_bound},
        {'type': 'ineq', 'fun': vega_upper_bound}
    ]

    # Optimization
    result = minimize(objective, x0, bounds=bounds, constraints=constraints, method='SLSQP')

    # Check if optimization was successful
    if not result.success:
        raise ValueError("Optimization failed: " + result.message)

    return result.x


def calculate_portfolio_metrics(weights, pnl, dv01_vectors, vega_vectors):
    portfolio_pnl = np.dot(weights, pnl)
    portfolio_dv01 = np.dot(weights, dv01_vectors)
    portfolio_vega = np.dot(weights, vega_vectors)

    return portfolio_pnl, portfolio_dv01, portfolio_vega


def main():
    pnl = np.array([100, 120, 90, 130, 110, 140])
    dv01_vectors = np.array([
        [0.1, -0.2, 0.15, 0.1],
        [0.2, 0.1, -0.25, 0.3],
        [0.3, -0.2, 0.1, 0.25],
        [-0.25, 0.35, 0.2, -0.3],
        [0.15, -0.25, 0.3, 0.1],
        [0.2, 0.15, -0.35, 0.25]
    ])
    vega_vectors = np.array([
        [0.05, 0.2, -0.1, 0.15],
        [-0.15, 0.1, 0.2, -0.25],
        [0.2, -0.15, 0.1, 0.2],
        [0.25, 0.3, -0.2, 0.1],
        [0.1, -0.25, 0.3, -0.2],
        [0.2, 0.1, -0.25, 0.3]
    ])
    dv01_min = -2.0
    dv01_max = 2.0
    vega_min = -1.5
    vega_max = 1.5

    optimal_weights = optimize_portfolio_pnl(pnl, dv01_vectors, vega_vectors, dv01_min, dv01_max, vega_min, vega_max)
    portfolio_pnl, portfolio_dv01, portfolio_vega = calculate_portfolio_metrics(optimal_weights, pnl, dv01_vectors, vega_vectors)
    print("Optimal Weights:", optimal_weights)
    print("Portfolio PnL:", portfolio_pnl)
    print("Portfolio dv01:", portfolio_dv01)
    print("Portfolio Vega:", portfolio_vega)

if __name__ == "__main__":
    main()
