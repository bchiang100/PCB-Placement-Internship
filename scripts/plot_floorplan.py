#!/usr/bin/env python3
import matplotlib.pyplot as plt
import matplotlib.patches as patches
import sys
import random

#USAGE: python3 plot_floorplan.py output.rpt
def parse_output_file(filename):
    """Parse the output.rpt file and extract macro information."""
    with open(filename, 'r') as f:
        lines = f.readlines()
    
    # Parse header information
    final_cost = float(lines[0].strip())
    total_wirelength = int(lines[1].strip())
    chip_area = int(lines[2].strip())
    chip_dims = lines[3].strip().split()
    chip_width = int(chip_dims[0])
    chip_height = int(chip_dims[1])
    runtime = float(lines[4].strip())
    
    # Parse macro positions
    macros = []
    for i in range(5, len(lines)):
        parts = lines[i].strip().split()
        if len(parts) == 5:
            macro = {
                'name': parts[0],
                'x1': int(parts[1]),
                'y1': int(parts[2]),
                'x2': int(parts[3]),
                'y2': int(parts[4]),
                'width': int(parts[3]) - int(parts[1]),
                'height': int(parts[4]) - int(parts[2])
            }
            macros.append(macro)
    
    return {
        'final_cost': final_cost,
        'total_wirelength': total_wirelength,
        'chip_area': chip_area,
        'chip_width': chip_width,
        'chip_height': chip_height,
        'runtime': runtime,
        'macros': macros
    }

def plot_floorplan(data, output_filename=None):
    """Create a visualization of the floorplan."""
    fig, ax = plt.subplots(1, 1, figsize=(12, 10))
    
    # Set up the plot
    ax.set_xlim(0, data['chip_width'])
    ax.set_ylim(0, data['chip_height'])
    ax.set_aspect('equal')
    ax.invert_yaxis()  # Invert y-axis to match typical floorplan orientation
    
    # Generate colors for each macro
    colors = plt.cm.Set3(range(len(data['macros'])))
    
    # Plot each macro
    for i, macro in enumerate(data['macros']):
        # Create rectangle
        rect = patches.Rectangle(
            (macro['x1'], macro['y1']),
            macro['width'],
            macro['height'],
            linewidth=1,
            edgecolor='black',
            facecolor=colors[i % len(colors)],
            alpha=0.7
        )
        ax.add_patch(rect)
        
        # Add macro name in the center
        cx = macro['x1'] + macro['width'] / 2
        cy = macro['y1'] + macro['height'] / 2
        ax.text(cx, cy, macro['name'], 
                ha='center', va='center', 
                fontsize=8, weight='bold')
    
    # Draw chip outline
    chip_outline = patches.Rectangle(
        (0, 0),
        data['chip_width'],
        data['chip_height'],
        linewidth=3,
        edgecolor='red',
        facecolor='none'
    )
    ax.add_patch(chip_outline)
    
    # Add grid
    ax.grid(True, alpha=0.3)
    
    # Set labels and title
    ax.set_xlabel('X coordinate')
    ax.set_ylabel('Y coordinate')
    ax.set_title(f'Floorplan Visualization\n' + 
                 f'Chip: {data["chip_width"]}x{data["chip_height"]}, ' +
                 f'Cost: {data["final_cost"]:.2f}, ' +
                 f'Runtime: {data["runtime"]:.2f}s')
    
    # Add statistics
    total_macro_area = sum(m['width'] * m['height'] for m in data['macros'])
    utilization = (total_macro_area / data['chip_area']) * 100
    
    stats_text = (f'Total Macros: {len(data["macros"])}\n'
                  f'Chip Area: {data["chip_area"]}\n'
                  f'Macro Area: {total_macro_area}\n'
                  f'Utilization: {utilization:.1f}%\n'
                  f'Wirelength: {data["total_wirelength"]}')
    
    ax.text(0.02, 0.98, stats_text,
            transform=ax.transAxes,
            verticalalignment='top',
            fontsize=10,
            bbox=dict(boxstyle='round', facecolor='wheat', alpha=0.5))
    
    plt.tight_layout()
    
    if output_filename:
        plt.savefig(output_filename, dpi=300, bbox_inches='tight')
        print(f"Saved plot to {output_filename}")
    
    plt.show()

def check_overlaps(macros):
    """Check if any macros overlap."""
    overlaps = []
    for i in range(len(macros)):
        for j in range(i + 1, len(macros)):
            m1, m2 = macros[i], macros[j]
            # Check if rectangles overlap
            if not (m1['x2'] <= m2['x1'] or m1['x1'] >= m2['x2'] or
                    m1['y2'] <= m2['y1'] or m1['y1'] >= m2['y2']):
                overlaps.append((m1['name'], m2['name']))
    return overlaps

def main():
    if len(sys.argv) < 2:
        print("Usage: python plot_floorplan.py <output.rpt> [output_image.png]")
        sys.exit(1)
    
    input_file = sys.argv[1]
    output_file = sys.argv[2] if len(sys.argv) > 2 else None
    
    # Parse the file
    try:
        data = parse_output_file(input_file)
    except Exception as e:
        print(f"Error reading file: {e}")
        sys.exit(1)
    
    # Check for overlaps
    overlaps = check_overlaps(data['macros'])
    if overlaps:
        print("Warning: Overlapping macros detected:")
        for m1, m2 in overlaps:
            print(f"  {m1} overlaps with {m2}")
    else:
        print("No overlapping macros detected.")
    
    # Check if macros fit within chip bounds
    out_of_bounds = []
    for macro in data['macros']:
        if macro['x2'] > data['chip_width'] or macro['y2'] > data['chip_height']:
            out_of_bounds.append(macro['name'])
    
    if out_of_bounds:
        print("Warning: Macros exceeding chip boundaries:")
        for name in out_of_bounds:
            print(f"  {name}")
    else:
        print("All macros fit within chip boundaries.")
    
    # Plot the floorplan
    plot_floorplan(data, output_file)

if __name__ == "__main__":
    main()