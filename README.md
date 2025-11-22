# Snake game GBA

This is a simple approach to code a snake game... for the GBA. Meaning that this should run on any emulator, or in a real GBA, although I don't have one in hands :(

## Toolset

I'm using the DevkitPRO toolset for GBA game development (`gba-dev`). One downside is that you can only install it via pacman, but I use the mighty and powerful OpenSUSE, **by the way**. So I made a Dockerfile for a light Arch image that can compile the source code.

You can build it via:

`sudo docker build -t gba-builder .`

And then compile the sourcecode via:

`docker run --rm -it   -v $(pwd):/project   -w /project   gba-builder make`

And this should work no problem.
