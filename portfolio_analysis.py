import numpy as np
import pandas as pd

np.random.seed(42)  # For reproducibility

# Generating sample data
trade_ids = np.arange(1, 16)  # 15 trades
dv01s = np.random.normal(0, 0.03, 15)  # Normal distribution, mean = 0, std = 0.03
vegas = dv01s * np.random.normal(1, 0.5, 15) + np.random.normal(0, 1, 15)  # Controlled correlation with some randomness
pvs = np.random.normal(0, 200000, 15)  # Generate PVs with large spread

# Create DataFrame
portfolio_df = pd.DataFrame({
    'TradeID': trade_ids,
    'DV01': dv01s,
    'Vega': vegas,
    'PV': pvs
})

# Save to CSV
file_path = 'portfolio_data_with_randoms.csv'
portfolio_df.to_csv(file_path, index=False)
print("Generated data saved to", file_path)

