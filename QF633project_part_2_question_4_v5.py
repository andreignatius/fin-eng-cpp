import numpy as np
from scipy.optimize import minimize
import json



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
    # Load the JSON file
    with open('../../output/output_20240703_011545.json', 'r') as file:
        data = json.load(file)

    # Extract the relevant parts
    trades = data["2024-06-02"]
    pnl = data["PnL"]

    # Sort the trades by keys
    sorted_trades = {k: trades[k] for k in sorted(trades, key=int)}

    # Initialize lists to store DV01, Vega, and PnL values
    dv01_list = []
    vega_list = []
    pnl_list = []

    # Iterate through sorted trades to extract DV01 and Vega values
    for trade_id, trade_data in sorted_trades.items():
        dv01_values = list(trade_data["DV01"].values())
        vega_values = list(trade_data["Vega"].values()) if trade_data["Vega"] is not None else [0] * len(dv01_values)
        
        dv01_list.append(dv01_values)
        vega_list.append(vega_values)
        pnl_list.append(pnl[trade_id])

    # Convert lists to numpy arrays
    dv01_vectors = np.array(dv01_list)
    vega_vectors = np.array(vega_list)
    pnl = np.array(pnl_list)
    

    dv01_min = -2.0
    dv01_max = 2.0
    vega_min = -1.5
    vega_max = 1.5

    def scale_vectors(vectors):
        norms = np.linalg.norm(vectors, axis=0)
        norms[norms == 0] = 1  # Prevent division by zero
        return vectors / norms

    dv01_vectors = scale_vectors(dv01_vectors)
    vega_vectors = scale_vectors(vega_vectors)

    optimal_weights = optimize_portfolio_pnl(pnl, dv01_vectors, vega_vectors, dv01_min, dv01_max, vega_min, vega_max)
    portfolio_pnl, portfolio_dv01, portfolio_vega = calculate_portfolio_metrics(optimal_weights, pnl, dv01_vectors,
                                                                                vega_vectors)
    print("Optimal Weights:", optimal_weights)
    print("Portfolio PnL:", portfolio_pnl)
    print("Portfolio dv01:", portfolio_dv01)
    print("Portfolio Vega:", portfolio_vega)


if __name__ == "__main__":
    main()
