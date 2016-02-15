/*
 * make.cpp
 *
 *  Created on: 13.02.2016
 *      Author: mike_gresens
 */

#include "make.hpp"
#include "model.hpp"
#include "../instance.hpp"
#include <boost/regex/pending/unicode_iterator.hpp>
#include <boost/log/trivial.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_OUTLINE_H
#include FT_BBOX_H
#include FT_ERRORS_H

namespace rt {
namespace surface {
namespace primitive {
namespace text {

constexpr float factor = 1e-3f;

class Decomposer
{
private:
	FT_Outline_Funcs		_functions;
	std::vector<contour_t>	_contours;
	vector_t				_fromPoint;
	float					_advanceX;
	float					_advanceY;

protected:
	static int
	moveToFunc(const FT_Vector* to, void* user)
	{
		Decomposer* decomposer = reinterpret_cast<Decomposer*>(user);
		decomposer->_fromPoint = makePoint(*to, decomposer->_advanceX, decomposer->_advanceY);
		return 0;
	}

	static int
	lineToFunc(const FT_Vector* to, void* user)
	{
		Decomposer* decomposer = reinterpret_cast<Decomposer*>(user);
		const vector_t toPoint = makePoint(*to, decomposer->_advanceX, decomposer->_advanceY);
		const bezier1_t bezier {{decomposer->_fromPoint, toPoint}};
		decomposer->_contours.push_back(std::make_tuple(to_polynomial(bezier), to_polynomial(differentiate(bezier))));
		decomposer->_fromPoint = toPoint;
		return 0;
	}

	static int
	conicToFunc(const FT_Vector* c1, const FT_Vector* to, void* user)
	{
		Decomposer* decomposer = reinterpret_cast<Decomposer*>(user);
		const vector_t c1Point = makePoint(*c1, decomposer->_advanceX, decomposer->_advanceY);
		const vector_t toPoint = makePoint(*to, decomposer->_advanceX, decomposer->_advanceY);
		const bezier2_t bezier {{decomposer->_fromPoint, c1Point, toPoint}};
		decomposer->_contours.push_back(std::make_tuple(to_polynomial(bezier), to_polynomial(differentiate(bezier))));
		decomposer->_fromPoint = toPoint;
		return 0;
	}

	static int
	cubicToFunc(const FT_Vector* c1, const FT_Vector* c2, const FT_Vector* to, void* user)
	{
		Decomposer* decomposer = reinterpret_cast<Decomposer*>(user);
		const vector_t c1Point = makePoint(*c1, decomposer->_advanceX, decomposer->_advanceY);
		const vector_t c2Point = makePoint(*c2, decomposer->_advanceX, decomposer->_advanceY);
		const vector_t toPoint = makePoint(*to, decomposer->_advanceX, decomposer->_advanceY);
		const bezier3_t bezier {{decomposer->_fromPoint, c1Point, c2Point, toPoint}};
		decomposer->_contours.push_back(std::make_tuple(to_polynomial(bezier), to_polynomial(differentiate(bezier))));
		decomposer->_fromPoint = toPoint;
		return 0;
	}

	static vector_t
	makePoint(const FT_Vector& point, const float advanceX, const float advanceY)
	{
		return vector_t {advanceX + point.x, advanceY + point.y, 0, 0} * factor;
	}

public:
	Decomposer(const float advanceX, const float advanceY)
	:
		_functions({moveToFunc, lineToFunc, conicToFunc, cubicToFunc}),
		_contours(),
		_fromPoint {0, 0, 0, 0},
		_advanceX(advanceX),
		_advanceY(advanceY)
	{
	}

	std::vector<contour_t>
	operator()(FT_Outline* outline)
	{
		const FT_Error error = FT_Outline_Decompose(outline, &_functions, this);
		if (error)
			throw std::runtime_error("Can't decompose outline. Code = " + std::to_string(error));
		return _contours;
	}
};

typedef std::shared_ptr<FT_LibraryRec_> Library;
typedef std::shared_ptr<FT_FaceRec_> Face;
typedef std::shared_ptr<FT_GlyphRec> Glyph;

static Library
init_library()
{
	FT_Library handle = nullptr;
	const FT_Error error = FT_Init_FreeType(&handle);
	const Library library(handle, FT_Done_FreeType);
	if (error)
		throw std::runtime_error("Can't initiate freetype library. Code = " + std::to_string(error));
	return library;
}

static Face
load_face(const Library& library, const std::string& font)
{
	FT_Face handle = nullptr;
	const FT_Error error = FT_New_Face(library.get(), font.data(), 0, &handle);
	const Face face(handle, FT_Done_Face);
	if (error)
		throw std::runtime_error("Can't load freetype face '" + font + "'. Code = " + std::to_string(error));
	return face;
}

static Glyph
get_glyph(const Face& face, const char32_t ch, const std::size_t sizeX, const std::size_t sizeY, const std::size_t resolutionX, const std::size_t resolutionY)//, const double advanceX)
{
	FT_Error error = FT_Set_Char_Size(face.get(), sizeX << 6, sizeX << 6, resolutionX, resolutionY);
	if (error)
		throw std::runtime_error("Can't set glyph size. Code = " + std::to_string(error));

	const FT_UInt index = FT_Get_Char_Index(face.get(), ch);

	error = FT_Load_Glyph(face.get(), index, FT_LOAD_DEFAULT);
	if (error)
		throw std::runtime_error("Can't load glyph. Code = " + std::to_string(error));

	if (face->glyph->format != FT_GLYPH_FORMAT_OUTLINE)
		throw std::runtime_error("Glyph is not outlined.");

    FT_Glyph handle = nullptr;
    error = FT_Get_Glyph(face->glyph, &handle);
	const Glyph glyph(handle, FT_Done_Glyph);
	if (error)
		throw std::runtime_error("Can't get glyph. Code = " + std::to_string(error));
	return glyph;
}

instance_t
make(const description_t& description)
{
	BOOST_LOG_TRIVIAL(debug) << "Make surface text";

	const std::u32string text
	(
		boost::u8_to_u32_iterator<std::string::const_iterator>(description.text.cbegin()),
		boost::u8_to_u32_iterator<std::string::const_iterator>(description.text.cend())
	);

	const Library library = init_library();
	const Face face = load_face(library, description.font);

	float xadvance = 0.0f;
	float yadvance = 0.0f;
	glyphs_t glyphs;
	rtree_t rtree;
	std::size_t hits = 0;

	for (std::size_t i = 0, n = 0; i < text.size(); ++i)
	{
		const char32_t ch = text[i];

		if (ch != '?')
		{
		const Glyph glyph = get_glyph(face, ch, 25, 25, 300, 300);//, xadvance);
		const FT_Outline* outline = &reinterpret_cast<FT_OutlineGlyph>(glyph.get())->outline;

		FT_BBox bbox;
		const FT_Error error = FT_Outline_Get_BBox(const_cast<FT_Outline*>(outline), &bbox);
		if (error)
			throw std::runtime_error("Can't get bbox. Code = " + std::to_string(error));

		constexpr float delta = 1e-3f;
		const box_t box
		(
			point_t ((bbox.xMin + xadvance) * factor - delta, (bbox.yMin + yadvance) * factor - delta,  -delta),
			point_t ((bbox.xMax + xadvance) * factor + delta, (bbox.yMax + yadvance) * factor + delta, description.extrusion + delta)
		);

		Decomposer decompose(xadvance, yadvance);
		contours_t contours = decompose(const_cast<FT_Outline*>(outline));

		xadvance += face->glyph->metrics.horiAdvance;

		glyphs.emplace_back(std::move(contours), description.extrusion);
		rtree.insert(value_t(box, n));
		hits += (3 * contours.size() + 2);
		n++;
		}
		else
		{
			xadvance = 0;
			yadvance -= face->glyph->metrics.vertAdvance;
		}
	}

	model_t model(std::move(glyphs), std::move(rtree), description.extrusion);
	return std::make_shared<instance_impl_t<model_t>>(description.transformation, std::move(model));
}

}
}
}
}
