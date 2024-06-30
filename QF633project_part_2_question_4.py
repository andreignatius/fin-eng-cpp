import numpy as np
from scipy.optimize import minimize

def neutralize_risks(coefficients):
    """
    Calculate the weights required to neutralize DV01 and Vega for a given set of tradable assets.

    Parameters:
    coefficients (np.ndarray): A 2D numpy array where each row represents the coefficients for a specific risk
                               (e.g., DV01 and Vega) and each column represents a tradable asset.

    Returns:
    np.ndarray: A numpy array containing the weights for each tradable asset to neutralize the given risks.
    """
    num_assets = coefficients.shape[1]

    # Set the weight of the last asset to 1
    coefficients_reduced = coefficients[:, :num_assets-1]
    rhs = -coefficients[:, num_assets-1]

    # Solve for the other weights
    weights_reduced = np.linalg.solve(coefficients_reduced, rhs)

    # Complete the weights vector
    weights = np.append(weights_reduced, 1)

    return weights

def optimize_portfolio(coefficients, PnL, target_DV01, target_Vega, risk_level=1.0):
    """
    Optimize the portfolio to maximize PnL given a fixed risk limit of DV01 and Vega.

    Parameters:
    coefficients (np.ndarray): A 2D numpy array where each row represents the coefficients for a specific risk
                               (e.g., DV01 and Vega) and each column represents a tradable asset.
    PnL (np.ndarray): A 1D numpy array representing the PnL contributions of each tradable asset.
    target_DV01 (float): The target DV01 value for the portfolio.
    target_Vega (float): The target Vega value for the portfolio.
    risk_level (float, optional): A multiplier for the risk limits, default is 1.0.

    Returns:
    tuple: Optimal weights for each tradable asset and the maximum PnL achieved.
    """
    num_assets = coefficients.shape[1]

    # Objective function: Negative PnL to maximize it
    def objective(weights):
        return -np.dot(PnL, weights)

    # Constraint functions
    def constraint_DV01(weights):
        return np.dot(coefficients[0], weights) - target_DV01 * risk_level

    def constraint_Vega(weights):
        return np.dot(coefficients[1], weights) - target_Vega * risk_level

    # Initial guess for the weights
    initial_guess = np.ones(num_assets) / num_assets

    # Constraints dictionary
    constraints = [
        {'type': 'eq', 'fun': constraint_DV01},
        {'type': 'eq', 'fun': constraint_Vega}
    ]

    # Bounds for the weights (assuming non-negative weights)
    bounds = [(0, None) for _ in range(num_assets)]

    # Perform the optimization
    result = minimize(objective, initial_guess, method='SLSQP', constraints=constraints, bounds=bounds)

    # Check for optimization success
    if result.success:
        weights = result.x
        max_return = -result.fun  # Since we minimized the negative PnL
        return weights, max_return
    else:
        return None, None

# Example usage:
coefficients = np.array([
    [100, -150, 200],  # DV01 coefficients for Bond A, Option B, and Swap C
    [50, 70, -30]      # Vega coefficients for Bond A, Option B, and Swap C
])
PnL = np.array([10, 20, 15])  # PnL for Bond A, Option B, and Swap C
target_DV01 = 0.3
target_Vega = 0.2
risk_level = 1.0  # User-defined risk level

# Calculate risk-neutral weights
weights_neutral = neutralize_risks(coefficients)
print("Weights to neutralize DV01 and Vega:", weights_neutral)

# Optimize portfolio
weights, max_return = optimize_portfolio(coefficients, PnL, target_DV01, target_Vega, risk_level)
if weights is not None:
    print("Optimal weights to maximize PnL while maintaining DV01 and Vega constraints:")
    for i, weight in enumerate(weights):
        print(f"Weight for Asset {chr(65 + i)}: {weight}")  # Using chr(65 + i) to convert 0 to 'A', 1 to 'B', etc.
    print(f"Maximum Return (PnL): {max_return}")
else:
    print("Optimization failed.")
