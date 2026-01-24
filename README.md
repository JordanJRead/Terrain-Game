## Overview

A fractional brownian motion-based infinite terrain generator with ray-marched atmospheric rendering that uses LOD, plane chunking, frustum culling, and terrain pre-computation in order to run at about 50 FPS (on my devices). It also uses shell texturing for rendering grass, sun of sines to render water, and many different levels of perlin noise to create different geological features. It also has parameters for almost every aspect of the rendering and terrain.

This project is an extension of another project I made for the Acerole Dirt Jam (https://itch.io/jam/acerola-dirt-jam), a terrain generation-themed game jam taking place over one month. My original project is here: https://github.com/JordanJRead/DirtJamSubmission.

Skybox(es) for debug mode from https://assetstore.unity.com/packages/2d/textures-materials/sky/allsky-free-10-sky-skybox-set-146014
### Controls
WASD for movement, space to go up, left shift or C to go down, H to toggle the GUI, R to toggle the cursor, and hold Q and E to change the camera's speed.
### Images

<img width="1908" height="1070" alt="Daytime standard" src="https://github.com/user-attachments/assets/83799842-58d2-458f-9621-9686e5b7c4dd" />

<img width="1904" height="1068" alt="Nighttime standard" src="https://github.com/user-attachments/assets/ea789f5d-2b02-4274-8e3c-b45bed5ae447" />

<img width="1905" height="1072" alt="Sunset" src="https://github.com/user-attachments/assets/12ab9204-4bd5-41a5-a62a-382a023a83f7" />

<img width="1911" height="1070" alt="Daytime grass" src="https://github.com/user-attachments/assets/0eb5acd6-2d40-4316-acec-045f8264da7a" />

<img width="1902" height="1064" alt="Daytime mountain top" src="https://github.com/user-attachments/assets/ccfc40f2-72f4-4657-93f6-aa527a70c167" />

<img width="1902" height="1064" alt="Daytime lake view" src="https://github.com/user-attachments/assets/f8e25258-6659-4ca6-88da-a3ada3e6e1cc" />

<img width="1909" height="1068" alt="Nighttime lake view" src="https://github.com/user-attachments/assets/0c24fa51-618b-4364-86c9-48928e3816f3" />

<img width="1910" height="1069" alt="Daytime sprawling" src="https://github.com/user-attachments/assets/970e8573-c836-4c3f-a1a7-091f6fcd2b82" />

<img width="1915" height="1071" alt="Evening sprawling" src="https://github.com/user-attachments/assets/37b177e9-b79f-43f4-8b9d-cb108b046764" />

<img width="1907" height="1067" alt="Nighttime sprawling" src="https://github.com/user-attachments/assets/b956f48f-b421-4942-9c65-bbf60323332a" />

All of the above images, except for the last three, give me around 50 FPS

<img width="1907" height="1070" alt="UI jumpscare" src="https://github.com/user-attachments/assets/7781d81e-8393-401b-a8d2-b59b99b7153e" />

<img width="1906" height="1065" alt="Debug mode" src="https://github.com/user-attachments/assets/2aad4665-869e-4f28-bd63-bd3d3dc620f2" />
