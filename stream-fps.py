import cv2
import pygame
import requests
import time
import os
import csv
from datetime import datetime
import math
import numpy as np

# Initialize Pygame
pygame.init()

# Set up the HTTP address for sending commands
CAR_IP = "http://192.168.4.1"

# Set up video capture
cap = cv2.VideoCapture(f'{CAR_IP}:81/stream')

# Get video properties
frame_width = int(cap.get(cv2.CAP_PROP_FRAME_WIDTH))
frame_height = int(cap.get(cv2.CAP_PROP_FRAME_HEIGHT))
fps = int(cap.get(cv2.CAP_PROP_FPS))

# Initialize control values
steering = 128  # Neutral position
motor = 0  # Stopped

# Recording variables
is_recording = False
frame_count = 0
record_folder = ""
csv_file = None
csv_writer = None

# Pygame window setup
WINDOW_WIDTH = 800
WINDOW_HEIGHT = 480
window = pygame.display.set_mode((WINDOW_WIDTH, WINDOW_HEIGHT))
pygame.display.set_caption("FPV Car Control")

# Colors
BLACK = (0, 0, 0)
WHITE = (255, 255, 255)
RED = (255, 0, 0)
GREEN = (0, 255, 0)

# Initialize font for FPS display
font = pygame.font.Font(None, 36)

# FPS calculation variables
fps_start_time = time.time()
fps_counter = 0
current_fps = 0

def calculate_fps():
    global fps_start_time, fps_counter, current_fps
    fps_counter += 1
    if time.time() - fps_start_time > 1.0:
        current_fps = fps_counter
        fps_counter = 0
        fps_start_time = time.time()
    return current_fps

def draw_control_visualization(frame):
    window.fill(BLACK)
    
    # Convert frame to pygame surface and display in center
    frame = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
    frame = np.rot90(frame)
    frame = pygame.surfarray.make_surface(frame)
    frame_rect = frame.get_rect(center=(WINDOW_WIDTH//2, WINDOW_HEIGHT//2))
    window.blit(frame, frame_rect)
        
    # Display recording status
    if is_recording:
        text = font.render("Recording", True, RED)
        window.blit(text, (10, 10))
    
    # Display FPS
    fps = calculate_fps()
    fps_text = font.render(f"FPS: {fps}", True, GREEN)
    window.blit(fps_text, (10, 40))
    
    pygame.display.flip()

# Main loop
running = True
clock = pygame.time.Clock()

while running:
    # Capture video frame
    ret, frame = cap.read()
    if ret:
        # Update visualization
        draw_control_visualization(frame)
    
    # Control frame rate
    #clock.tick(30)

# Clean up
cap.release()
pygame.quit()
