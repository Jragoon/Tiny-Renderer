#include <vector>
#include <cmath>
#include <memory>
#include "tgaimage.h"
#include "geometry.h"
#include "model.h"

#define WIDTH  800
#define HEIGHT 800

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0, 0, 255);
const TGAColor blue  = TGAColor(0, 0, 255, 255);

bool transpose_if_steep(int& x0, int& y0, int& x1, int& y1) {
	if (std::abs(x0-x1) < std::abs(y0-y1)) {
		std::swap(x0, y0);
		std::swap(x1, y1);
		return true;
	}
	return false;
}

void force_left_to_right(int& x0, int& y0, int& x1, int& y1) {
	if (x0 > x1) {
		std::swap(x0, x1);
		std::swap(y0, y1);
	}
}

void draw_line(int x0, int y0, int x1, int y1, TGAImage& image, const TGAColor& color) {
	bool is_steep = transpose_if_steep(x0, y0, x1, y1);
	force_left_to_right(x0, y0, x1, y1);
	int divisor_x = x1 - x0;
	int divisor_y = y1 - y0;
	int divisor_err_2 = std::abs(divisor_y) * 2;
	int err_2 = 0;
	int y = y0;
	for (int x = x0; x < x1; ++x) {
		if (is_steep) image.set(y, x, color);
		else image.set(x, y, color);
		err_2 += divisor_err_2;
		if (err_2 > divisor_x) {
			y += y1 > y0 ? 1 : -1;
			err_2 -= divisor_x * 2;
		}
	}
}

void draw_wireframe(TGAImage& image, Model* model) {
	for (int i = 0; i < model->nfaces(); ++i) {
		std::vector<int> face = model->face(i);
		for (int j = 0; j < 3; ++j) {
			Vec3f v0 = model->vert(face[j]);
			Vec3f v1 = model->vert(face[(j+1)%3]);
			int x0 = (v0.x + 1.0) * WIDTH / 2.0;
			int y0 = (v0.y + 1.0) * HEIGHT / 2.0;
			int x1 = (v1.x + 1.0) * WIDTH / 2.0;
			int y1 = (v1.y + 1.0) * HEIGHT / 2.0;
			draw_line(x0, y0, x1, y1, image, white);
		}
	}
}

int main(int argc, char* argv[]) {
	std::unique_ptr<Model> test;
	if (argc == 2) test = std::make_unique<Model>(argv[1]);
	else test = std::make_unique<Model>("../src/obj/diablo3_pose/diablo3_pose.obj");

	TGAImage image(WIDTH, HEIGHT, TGAImage::RGB);
	draw_wireframe(image, test.get());
	image.flip_vertically();
	image.write_tga_file("output.tga");
	return 0;
}