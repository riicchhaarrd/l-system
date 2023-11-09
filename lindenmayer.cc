#include <cfloat>
#include <cmath>
#include <cstring>
#include <iostream>
#include <map>
#include <sstream>
#include <stack>
#include <vector>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

struct Vec2
{
	float x = 0.f;
	float y = 0.f;
	Vec2()
	{
	}
	Vec2(float x_) : x(x_), y(x_)
	{
	}
	Vec2(float x_, float y_) : x(x_), y(y_)
	{
	}
	Vec2 operator+(const Vec2& o)
	{
		return Vec2(x + o.x, y + o.y);
	}
	Vec2 operator-(const Vec2& o)
	{
		return Vec2(x - o.x, y - o.y);
	}
	Vec2 operator*(const Vec2& o)
	{
		return Vec2(x * o.x, y * o.y);
	}
	Vec2 operator/(const Vec2& o)
	{
		return Vec2(x / o.x, y / o.y);
	}
};

struct RGB
{
	unsigned char r = 0;
	unsigned char g = 0;
	unsigned char b = 0;
};

struct Image
{
	int width = 0;
	int height = 0;
	int channels = 0;
	std::vector<unsigned char> pixels;
	Image()
	{
	}
	Image(int w, int h, int c) : width(w), height(h), channels(c)
	{
		pixels.resize(w * h * c);
		memset(pixels.data(), 0, w * h * c);
	}
	Image(int w, int h, int c, std::vector<unsigned char>& data) : width(w), height(h), channels(c)
	{
		pixels.insert(pixels.end(), data.begin(), data.end());
	}

	unsigned char* data()
	{
		return pixels.data();
	}

	unsigned char safe_get(size_t index)
	{
		if (index >= pixels.size())
			return 0;
		return pixels[index];
	}

	void safe_set(size_t index, unsigned char c)
	{
		if (index >= pixels.size())
			return;
		pixels[index] = c;
	}

	void set(int x, RGB c)
	{
		if (channels > 0)
			safe_set(x * channels, c.r);
		if (channels > 1)
			safe_set(x * channels + 1, c.g);
		if (channels > 2)
			safe_set(x * channels + 2, c.b);
	}
	void set(int x, int y, RGB c)
	{
		int index = (y * width + x) * channels;
		if (channels > 0)
			safe_set(index, c.r);
		if (channels > 1)
			safe_set(index + 1, c.g);
		if (channels > 2)
			safe_set(index + 2, c.b);
	}

	void line(int x1, int y1, int x2, int y2, RGB c)
	{
		// Bresenham's Line Algorithm
		int dx = abs(x2 - x1);
		int dy = abs(y2 - y1);
		int sx = (x1 < x2) ? 1 : -1;
		int sy = (y1 < y2) ? 1 : -1;
		int err = dx - dy;

		while (true)
		{
			set(x1, y1, c);

			if (x1 == x2 && y1 == y2)
			{
				break;
			}

			int e2 = 2 * err;
			if (e2 > -dy)
			{
				err -= dy;
				x1 += sx;
			}

			if (e2 < dx)
			{
				err += dx;
				y1 += sy;
			}
		}
	}
};

struct State
{
	Vec2 position;
	float angle = 90.f;
};

struct TurtleInterface
{
	virtual void line(const Vec2& start, const Vec2& end) = 0;
};

#define DEG2RAD (M_PI / 180.f)

struct Turtle
{
	State state;
	TurtleInterface* interface = NULL;
	Turtle(TurtleInterface* ti) : interface(ti)
	{
	}
	void forward(float distance)
	{
		Vec2 start = state.position;
		state.position.x = state.position.x + distance * cosf(state.angle * DEG2RAD);
		state.position.y = state.position.y - distance * sinf(state.angle * DEG2RAD);
		interface->line(start, state.position);
	}

	void right(float theta)
	{
		state.angle += theta;
	}

	void left(float theta)
	{
		state.angle -= theta;
	}
};

struct TurtleTestInterface : TurtleInterface
{
	Vec2 max_point, min_point;
	TurtleTestInterface()
	{
		max_point.x = -FLT_MAX;
		max_point.y = -FLT_MAX;
		min_point.x = FLT_MAX;
		min_point.y = FLT_MAX;
	}
	virtual void line(const Vec2& start, const Vec2& end) override
	{
		if (start.x < min_point.x)
			min_point.x = start.x;
		if (start.y < min_point.y)
			min_point.y = start.y;
		if (start.x > max_point.x)
			max_point.x = start.x;
		if (start.y > max_point.y)
			max_point.y = start.y;
		if (end.x < min_point.x)
			min_point.x = end.x;
		if (end.y < min_point.y)
			min_point.y = end.y;
		if (end.x > max_point.x)
			max_point.x = end.x;
		if (end.y > max_point.y)
			max_point.y = end.y;
	}
};

struct TurtleDrawerInterface : TurtleInterface
{
	Image& image;
	RGB color;
	Vec2 offset;
	bool flip = false;
	TurtleDrawerInterface(Image& image_, RGB color_, Vec2 offset_) : image(image_), color(color_), offset(offset_)
	{
	}
	virtual void line(const Vec2& start, const Vec2& end) override
	{
		if (flip)
			image.line(image.width - (offset.x + start.x), offset.y + start.y, image.width - (offset.x + end.x),
					   offset.y + end.y, color);
		else
			image.line(offset.x + start.x, offset.y + start.y, offset.x + end.x, offset.y + end.y, color);
	}
};

void run_turtle(TurtleInterface* interface, const std::string& current_string, const std::map<char, std::string>& rules,
				int iterations, float angle, float length)
{
	Turtle turtle(interface);
	std::stack<Turtle> stack;

	for (char c : current_string)
	{
		if (c == 'F')
		{
			turtle.forward(length);
		}
		else if (c == '+')
		{
			turtle.right(angle);
		}
		else if (c == '-')
		{
			turtle.left(angle);
		}
		else if (c == '[')
		{
			stack.push(turtle);
		}
		else if (c == ']')
		{
			turtle = stack.top();
			stack.pop();
		}
	}
}

void apply_rules(std::string& axiom, const std::map<char, std::string>& rules)
{
	std::string result;
	for (char c : axiom)
	{
		auto it = rules.find(c);
		if (it != rules.end())
		{
			result += it->second;
		}
		else
		{
			result += c;
		}
	}
	axiom = result;
}

void render_l_system(const std::string& output_path, const std::string& axiom, const std::map<char, std::string>& rules,
					 int iterations, float angle, float length)
{
	std::string current_string = axiom;

	for (int i = 0; i < iterations; ++i)
	{
		apply_rules(current_string, rules);
	}

	printf("%s\n", current_string.c_str());
	TurtleTestInterface interface;
	run_turtle(&interface, current_string, rules, iterations, angle, length);

	printf("min = %f,%f, max = %f,%f\n", interface.min_point.x, interface.min_point.y, interface.max_point.x,
		   interface.max_point.y);
	int width = ceilf(fabs(interface.max_point.x - interface.min_point.x));
	int height = ceilf(fabs(interface.max_point.y - interface.min_point.y));
	printf("width = %d, height = %d\n", width, height);
	Image image(width, height, 3);
	RGB color = {255, 255, 255};
	Vec2 offset(-interface.min_point.x, -interface.min_point.y);
	TurtleDrawerInterface drawer_interface(image, color, offset);
	run_turtle(&drawer_interface, current_string, rules, iterations, angle, length);
	stbi_write_png(output_path.c_str(), width, height, 3, image.pixels.data(), 0);
}

int main(int argc, char** argv)
{
	std::string axiom = "X";
	std::map<char, std::string> rules;
	rules['F'] = "FF";
	rules['X'] = "F-[[X]+X]+F[+FX]-X";

	std::string output_path = "result.png";
	int iterations = 6;
	float angle = 22.5;
	float length = 5.0;
	if ((argc - 1) % 2 != 0)
	{
		fprintf(stderr, "Invalid arguments\n");
		return 1;
	}
	for (int i = 1; i < argc; i += 2)
	{
		std::string option = argv[i];
		if (option == "-axiom")
		{
			axiom = argv[i + 1];
		}
		else if (option == "-rules")
		{
			std::string rule;
			std::stringstream ss(argv[i + 1]);
			while (std::getline(ss, rule, ','))
			{
				auto f = rule.find_first_of('=');
				if (f == std::string::npos)
				{
					fprintf(stderr, "Error parsing rule, expected =\n");
					exit(1);
				}
				auto var = rule.substr(0, f);
				auto expr = rule.substr(f + 1);
				printf("Added rule '%s' = '%s'\n", var.c_str(), expr.c_str());
				rules[var[0]] = expr;
			}
		}
		else if (option == "-iterations")
		{
			iterations = std::stoi(argv[i + 1]);
		}
		else if (option == "-angle")
		{
			angle = std::stof(argv[i + 1]);
		}
		else if (option == "-length")
		{
			length = std::stof(argv[i + 1]);
		}
		else if (option == "-output")
		{
			output_path = argv[i + 1];
		}
	}

	render_l_system(output_path, axiom, rules, iterations, angle, length);

	return 0;
}
