import cv2 as cv
import numpy as np
from typing import Tuple

def closest_point_between_skew_lines(p1: np.ndarray, d1: np.ndarray, p2: np.ndarray, d2: np.ndarray) -> np.ndarray:
    """
    Finds the closest point between two skew lines in 3D space.
    
    Parameters:
        p1 (np.array): A point on the first line (3D vector)
        d1 (np.array): Direction vector of the first line (3D vector)
        p2 (np.array): A point on the second line (3D vector)
        d2 (np.array): Direction vector of the second line (3D vector)
    
    Returns:
        (np.array): The closest point to both lines
    """
    p1, d1, p2, d2 = map(np.asarray, (p1, d1, p2, d2))
    
    # Compute the normal to both direction vectors
    n = np.cross(d1, d2)
    n_norm = np.linalg.norm(n)
    
    if n_norm == 0: raise ValueError("The lines are parallel or coincident, no unique closest points.")
    
    n = n / n_norm  # Normalize normal
    
    # Compute the matrix components
    d1d1 = np.dot(d1, d1)
    d1d2 = np.dot(d1, d2)
    d2d2 = np.dot(d2, d2)
    
    # Compute the right-hand side
    rhs = p2 - p1
    rhs_d1 = np.dot(rhs, d1)
    rhs_d2 = np.dot(rhs, d2)
    
    # Solve for t and s (parameters for closest points)
    denom = d1d1 * d2d2 - d1d2 ** 2
    if abs(denom) < 1e-10: raise ValueError("The lines are nearly parallel, numerical instability may occur.")
    
    t = (rhs_d1 * d2d2 - rhs_d2 * d1d2) / denom
    s = (rhs_d1 * d1d2 - rhs_d2 * d1d1) / denom
    
    # Compute closest points
    closest_p1 = p1 + t * d1
    closest_p2 = p2 + s * d2
    
    return (closest_p1 + closest_p2)/2


def image_to_world_plane(u: float, v: float, H: cv.typing.MatLike) -> Tuple[float, float, float]:
    point = np.array([u, v, 1])
    projected = H @ point
    projected = projected / projected[2]  # Normalize
    return projected[0], projected[1], 0.0  # (X, Y, 0)
