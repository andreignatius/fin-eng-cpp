import numpy as np
from scipy.optimize import minimize


def minimize_risk(dv01, vega, min_weight=0.001):
    n = len(dv01)

    # Define Q matrix
    Q = np.outer(dv01, dv01) + np.outer(vega, vega)

    # Objective function
    def objective(w):
        return w.T @ Q @ w

    # Constraints
    constraints = [
        {'type': 'ineq', 'fun': lambda w: w - min_weight}  # Weights >= min_weight
    ]

    # Initial guess (equally weighted)
    initial_guess = np.ones(n) * min_weight

    # Solve the optimization problem
    result = minimize(objective, initial_guess, constraints=constraints)

    # Check if the solution is found
    if result.success:
        return result.x
    else:
        raise ValueError("Optimization failed: " + result.message)


import numpy as np
from cvxopt import matrix, solvers


def optimize_portfolio_qp(pnl, dv01, vega, dv01_limit, vega_limit, lambda_reg=0.01, min_weight=0.001):
    n = len(pnl)

    # Convert inputs to numpy arrays
    pnl = np.array(pnl)
    dv01 = np.array(dv01)
    vega = np.array(vega)

    # QP matrices
    P = matrix(2 * (lambda_reg * np.eye(n)), tc='d')
    q = matrix(-pnl, tc='d')

    # Inequality constraints for DV01, Vega, and non-negativity
    G = np.vstack([dv01, vega, -np.eye(n), np.eye(n)])
    h = np.array([dv01_limit, vega_limit] + [0.0] * n + [1.0] * n)

    # Additional constraints for minimum weight
    min_weight_constraints = -np.eye(n)
    min_weight_limits = np.array([-min_weight] * n)

    # Combine all constraints
    G = np.vstack([G, min_weight_constraints])
    h = np.hstack([h, min_weight_limits])

    # Convert to cvxopt matrices
    G = matrix(G, tc='d')
    h = matrix(h, tc='d')

    # Solve QP problem
    sol = solvers.qp(P, q, G, h)

    # Extract the optimal weights
    weights = np.array(sol['x']).flatten()

    # Post-process weights to enforce minimum weight
    weights[weights < min_weight] = 0.0

    return weights