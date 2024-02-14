#pragma once

#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <array>
#include <functional>	// std::function<> & std::hash<>
#include <cstdlib>		// std::rand

#if 0
#include <optional>
#else
#include "libs/Optional.h"	// custom made std::optional which can be replaced with the original one
#endif


#define DEBUG 1
#if DEBUG
	#include <iostream>	
	#include <string>
	#include "libs/Bmp.h"
	using std::cout;
	using std::cerr;
	using std::endl;
	#define println(x) std::cerr << #x << ": " << x << '\n'
	#define OUT(x, y)  std::cerr << x  << ": " << y << '\n'
#else // !DEBUG
	#define println(x)
	#define OUT(x, y)
#endif

// https://gist.github.com/jdah/ad997b858513a278426f8d91317115b9


namespace wfc { /* once finished, encapsulate everything in this namespace*/ };

enum Flag
{
	ROTATE = 1 << 0,	// 0001
	REFLECT = 1 << 1	// 0010
	// 0100
	// 1000
};
inline Flag operator|(Flag a, Flag b) { return static_cast<Flag>(static_cast<int>(a) | static_cast<int>(b)); }
inline Flag operator&(Flag a, Flag b) { return static_cast<Flag>(static_cast<int>(a) & static_cast<int>(b)); }

enum class ONFAILURE 
{
	CONTINUE,
	BACKTRACK,
	EXCEPTION,
	NUKE
};

enum class NEXTCELL 
{
	RANDOM,
	MIN_ENTROPY
};

enum class PATTERN
{
	WEIGHTED
};

// Datatype requirements:
// - default constructor -> duh
// - operator+(size_t) ---> for hashing

template <
	typename T,		// T: pattern datatype
	size_t D,		// D: dimensions
	size_t S		// S: pattern size
>
class WFC
{
	// constexprs
	static constexpr size_t pow(size_t s, size_t d) {
		for (size_t i = 1; i < d; i++) s *= s;
		return s;
	}

	static constexpr size_t VOL = pow(S, D);

	// Forward declarations
	template <size_t _D> struct _Rotator {};
	template <size_t _D> struct _Reflector {};
	template <size_t _D> struct _VectorUi {};
	struct Element;
	struct Pattern;
	struct Wave;

	// function to determine next element to collapse
	using NextCellFn = std::function<Element& (Wave&)>;

	//
	using PatternFn = std::function<size_t(Wave&, Element&)>;

	// function called onfailure
	using OnFailureFn = std::function<void()>; /* TODO */

	// function called for each iteration of the wave collapse
	using CallBackFn = std::function<void(const Wave&)>;

	// 2D operations
	template <>
	struct _VectorUi<2>
	{
		size_t x, y;
		_VectorUi() : x(0), y(0) {}
		_VectorUi(size_t _x, size_t _y) : x(_x), y(_y) {}
		_VectorUi(int _x, int _y) : x(_x), y(_y) {}
		bool operator==(const _VectorUi& other)
		{
			return (this->x == other.x && this->y == other.y);
		}
		bool operator!=(const _VectorUi& other)
		{
			return !(*this == other);
		}
		_VectorUi& operator=(const _VectorUi& other)
		{
			if (this != &other)
			{
				this->x = other.x;
				this->y = other.y;
			}
			return *this;
		}
		
		size_t volume() const { return this->x * this->y; }
		size_t toPosition(const _VectorUi& size) { return this->x + this->y * size.x; }

		void iterate(std::function<void(size_t x, size_t y)> fn)
		{
			for (size_t y = 0; y < this->y; y++) {
				for (size_t x = 0; x < this->x; x++) {
					fn(x, y);
				}
			}
		}
	};

	template <>
	struct _Rotator <2>
	{
		static void rotate(std::vector<Pattern>& patterns)
		{
			std::vector<Pattern> out;
			std::array<Pattern, 4> rotations;

			for (const Pattern& p : patterns)
			{
				rotations = rotateOne(p);
				out.insert(out.end(), rotations.begin(), rotations.end());
			}
			patterns = out;
		}
	private:
		static std::array<Pattern, 4> rotateOne(const Pattern& pattern)
		{
			std::array<Pattern, 4> out = {};
			out[0] = pattern;
			out[1] = rotate90(out[0]);
			out[2] = rotate90(out[1]);
			out[3] = rotate90(out[2]);
			return out;
		}

		static Pattern rotate90(const Pattern& pattern)
		{
			Pattern out;

			int c = 0;
			for (int y = 0, y_ = S; y < S; y++, y_--) {
				for (int x = 0, x1 = 1; x < S; x++, x1++) {

					int index = (y_ * x1) - 1 + (y * x);
					//std::cout << '[' << c << "] -> [" << index << "]\n";
					out.data[c] = pattern.data[index];
					c++;

				}
			}

			return out;
		}
	};

	template <>
	struct _Reflector <2>
	{
		static void reflect(std::vector<Pattern>& patterns)
		{
			std::vector<Pattern> out;
			std::array<Pattern, 4> reflections;

			for (const Pattern& p : patterns)
			{
				reflections = reflectOne(p);
				out.insert(out.end(), reflections.begin(), reflections.end());
			}

			patterns = out;
		}
	private:
		static std::array<Pattern, 4> reflectOne(const Pattern& pattern)
		{
			std::array<Pattern, 4> out;
			out[0] = pattern;
			out[1] = mirrorX(pattern);
			out[2] = mirrorY(pattern);
			out[3] = mirrorX(out[2]);
			return out;
		}

		static Pattern mirrorX(const Pattern& pattern)
		{
			Pattern out;
			int c = 0;
			for (int y = 0; y < S; y++) {
				for (int x = S; x > 0; x--) {

					int index = y * S + x - 1;
					//std::cout << '[' << c << "] -> [" << index << "]\n";
					out.data[c] = pattern.data[index];
					c++;
				}
			}
			return out;
		}

		static Pattern mirrorY(const Pattern& pattern)
		{
			Pattern out;

			int c = 0;
			for (int y = S; y > 0; y--) {
				for (int x = 0; x < S; x++) {

					int index = (y - 1) * S + x;
					//std::cout << '[' << c << "] -> [" << index << "]\n";
					out.data[c] = pattern.data[index];
					c++;
				}
			}

			return out;
		}
	};

	// 3D operations
	template<>
	struct _VectorUi<3> {};

	template<>
	struct _Rotator<3> {};

	template<>
	struct _Reflector<3> {};

	// Define operators for current dimension
	using Reflector = _Reflector<D>;
	using Rotator = _Rotator<D>;
	using Vector = _VectorUi<D>;

	// class/struct implementations
	struct Pattern
	{
		// value at center of the pattern
		T& value;

		// pattern data
		std::array<T, VOL> data;

		// for weight calculations
		float frequency;

		// hash value
		mutable size_t _hash;

		// valid/matching patterns
		/* TODO */
		
		// ctors
		Pattern() :
			value(this->data[VOL / 2]),
			frequency(1.f),
			_hash(0)
		{}

		Pattern(std::array<T, VOL> _data) :
			data(_data),
			value(_data[VOL / 2]),
			frequency(1.f),
			_hash(hash())
		{}

		// methods
		inline size_t hash() const
		{			
			if (_hash != 0xCCCCCCCC && _hash != 0)
				return _hash;

			// random shit go!
			size_t v = 0xabcdef;
			for (const T& data : this->data)
			{
				v ^= data + 0xfedcba + (v >> 4) + (v << 2);
			}
			this->_hash = v;
			return v;
		}

		// unused
		struct HashFn
		{
			size_t operator()(const Pattern& p) const
			{
				size_t v = 0xabcdef;
				for (const T& data : p.data)
				{
					v ^= data + 0xfedcba + (v >> 4) + (v << 2);
				}
				return v;
			}
		};

		// operators
		bool operator==(const Pattern& other)
		{
			return (this->hash() == other.hash());
		}

		Pattern& operator=(const Pattern& other)
		{
			if (this != &other)
			{
				this->data = other.data;
				this->frequency = other.frequency;
			}
			return *this;
		}

	#if DEBUG
		friend std::ostream& operator<<(std::ostream& os, const Pattern& pattern)
		{
			return os << (int)pattern._hash;
		}
	#endif
	};

	struct Element 
	{
		// value that element represents
		std::optional<T> value;

		// position of value in wave
		Vector position;

		// entropy value
		float entropy = 0.0f;

		Element() 
		{}

		Element(const Vector& _position) :
			position(_position)
		{}

		bool collapse();
	};

	struct Wave
	{
		// to access avilable patterns
		WFC& wfc;

		// unused
		Vector size;

		// all elements to-be/already collapsed
		std::vector<Element> wave;

		// amount of elements collapsed
		size_t n_collapsed;

		Wave(WFC& _wfc, Vector _size) :
			size(_size),
			wfc(_wfc),
			n_collapsed(0)
		{
			this->wave.reserve(this->size.volume());	// reserve wave size

			size.iterate([this](size_t x, size_t y) {	
				wave.push_back(Element({ x, y }));		// declare elements and set their positions
			});
		}

		std::vector<Element> collapse()
		{
			while (this->n_collapsed != this->wave.size())
			{

				n_collapsed++;
			}
			return this->wave;
		}
	};
	
	// functions
	OnFailureFn onFailure;
	NextCellFn nextCell;
	CallBackFn callBack;
	PatternFn  patternF;

	// Available patterns
	std::vector<Pattern> m_patterns;

	// input and output data
	std::vector<T> input, output;
	Vector input_size, output_size;

public:

	WFC() {}
	WFC(std::vector<T> input,
		Vector input_size,
		const Flag flags = static_cast<Flag>(0),
		NEXTCELL  _nextCell  = NEXTCELL::MIN_ENTROPY,
		PATTERN   _patternfn = PATTERN::WEIGHTED,
		ONFAILURE _onFailure = ONFAILURE::CONTINUE
	) :
		input(input),
		input_size(input_size)
	{
		// finds patterns
		auto findPatterns = [](T* data, const Vector size) -> std::vector<Pattern>
		{
			auto inBounds = [&](size_t x, size_t y) -> bool
			{
				return (x + S < size.x&& y + S < size.y);
			};

			std::vector<Pattern> patterns;

			for (size_t y = 0; y < size.y; y += S - 1) {
				for (size_t x = 0; x < size.x; x += S - 1) {

					Pattern	tmp;

					if (!inBounds(x, y)) continue;

					for (int _y = 0; _y < S; _y++) {
						for (int _x = 0; _x < S; _x++) {

							int data_index = (x + _x) + ((y + _y) * size.x);
							int copy_index = _x + _y * S;

							tmp.data[copy_index] = data[data_index];
						}
					}
					patterns.push_back(tmp);
				}
			}
			return patterns;
		};

		// removes duplicates | TODO: use std::unordered_set 
		auto deduplicate = [](std::vector<Pattern>& patterns) -> void
		{
			std::vector<Pattern> out;

			auto isInList = [&](const Pattern& p) -> bool {
				for (size_t i = 0; i < out.size(); i++)
					if (out[i] == p) return true;
				return false;
			};

			for (const Pattern& p : patterns)
			{
				if (!isInList(p)) out.push_back(p);
			}

			patterns = out;
		};

		// cannot use require() bc compiler version (must be +17)
		if (S % 2 == 0)
			throw "WFC<>::WFC(): -> S cannot be odd.";

		// function
		auto patterns = findPatterns(input.data(), input_size);
		deduplicate(patterns);

		if (flags & ROTATE)
		{
			Rotator::rotate(patterns);
			deduplicate(patterns);
		}

		if (flags & REFLECT)
		{
			Reflector::reflect(patterns);
			deduplicate(patterns);
		}

		// WFC DATA
		OUT("WFC info", ' ');
		OUT("- Dimension",      D);
		OUT("- Pattern size",   S);
		OUT("- Input size x",   input_size.x);
		OUT("- Input size y",   input_size.y);
		OUT("- Total patterns", patterns.size());
		OUT("- Flags", ((flags & ROTATE) ? ((flags & REFLECT) ? "ROTATE, REFLECT":"ROTATE") : ((flags & REFLECT) ? "ROTATE" : "")));	// dios perdoname por esto

		this->m_patterns = patterns;	// store patterns


		this->onFailure = this->CONTINUE();
		this->nextCell = this->MIN_ENTROPY();
		/* TODO */
		// set functions by flag
		//this->export_patterns(this->m_patterns);

	}

	void run(const Vector _size)
	{
		// set members
		this->output_size = _size;
		this->output.resize(_size.volume());

		bool hasFullyCollapsed = false;
		for (int y = 0;!hasFullyCollapsed; y++)
		{
			// select wave (series of elements to collapse)
			Wave wave(*this, { 1, (int)this->output_size.x });
			
			wave.collapse();
			// if wave.collapse().rval = -1 then FLAG_ONFAILURE
			
			// exit cycle
			break;
		}
		this->export_image(*this);
	}

	// -- DEBUG ONLY --
	#if DEBUG
	static void export_patterns(std::vector<Pattern> patterns)
	{
		bmp::Bitmap file(S, S);
		std::string filename;
		for (size_t i = 0; i < patterns.size(); i++)
		{
			auto data = patterns[i].data;
			file.setCastPixels(std::vector<T>(data.begin(), data.end()));
			filename = "output/patterns/pattern(" + std::to_string(i) + ").bmp";
			file.save(filename);
		}
	}

	static void export_image(const WFC& wfc, const char* path = "output/output.bmp")
	{
		bmp::Bitmap file(wfc.output_size.x, wfc.output_size.y);
		file.setCastPixels(wfc.output);
		file.save(path);
	}
	#else
	// let functions be undefined to avoid compilation
	static void export_patterns(std::vector<Pattern> patterns);
	static void export_image(const WFC& wfc, const char* filename = "output/output.bmp");
	#endif
private:
	OnFailureFn CONTINUE()
	{
		return [&]() { OUT("ONFAILURE", "<CONTINUE>"); };
	}
	
	NextCellFn MIN_ENTROPY()
	{
		return [&](Wave& w) -> Element&
		{
			Element* out = nullptr;
			float min = 0xffffff;
			for (auto& e:w.wave)
			{ 
				/*
				if (!e.isCollapsed() && e.entropy < min )
				{
					out = &e;
					min = e.entropy;
				}
				*/
			}
			return *out;
		};
	}
	NextCellFn RANDOM()
	{
		return [&](const Wave& w) -> Element&
		{
			auto randomNumber = [](int min, int max) { return std::rand() % max + min; };
			return w.wave[randomNumber(0, w.wave.size())];
		};
	}
};

