#include "pch.h"
#include "AssetsReader.h"

#include "tinyxml2.h"
#include "..//Assets.h"

namespace pe
{
	AssetsReader::AssetsReader(const char* path) {
		m_doc = new tinyxml2::XMLDocument();
		m_doc->LoadFile(path);
	}
	void AssetsReader::loadFile(const char* path) { m_doc->LoadFile(path); }

	void AssetsReader::readAssets(Assets& assets) {
		readTextures(assets.m_textures);
		readFonts(assets.m_fonts);
		readArea(assets.m_areas);
		readSprites(assets.m_sprites, &assets.m_textures);
		readBackground(assets.m_backgrounds, &assets.m_textures);
		readAnimation(assets.m_animations);
	}

	void AssetsReader::readTextures(std::map<int, Texture*>& texture_map) {
		auto textures = m_doc->FirstChildElement()->FirstChildElement("textures");
		for (auto texture_tag = textures->FirstChildElement(); texture_tag != NULL; texture_tag = texture_tag->NextSiblingElement()) {
			Texture* texture = new Texture();
			texture->setName(texture_tag->Attribute("name") );
			texture->m_id = texture_tag->IntAttribute("id"); // TODO: change this to assert line -> m_id == attr("id")
			texture->setSmooth(texture_tag->BoolAttribute("smooth") );
			texture->setRepeated(texture_tag->BoolAttribute("repeat") );
			std::string  path = texture_tag->GetText();
			if (path != "") {
				texture->loadFromFile(path);
			}
			texture_map[texture->getId()] = texture;
		}
	}

	void AssetsReader::readFonts(std::map<int, Font*>& font_map) {
		auto fonts = m_doc->FirstChildElement()->FirstChildElement("fonts");
		for (auto font_tag = fonts->FirstChildElement(); font_tag != NULL; font_tag = font_tag->NextSiblingElement()) {
			Font* font = new Font();
			font->setName(font_tag->Attribute("name"));
			font->m_id = font_tag->IntAttribute("id");
			std::string path = font_tag->GetText();
			if (path != "") {
				font->loadFromFile(path);
			}
			font_map[font->m_id] = font;
		}
	}

	void AssetsReader::readArea(std::map<int, Area*>& area_map) {
		auto areas = m_doc->FirstChildElement()->FirstChildElement("areas");
		for (auto area_tag = areas->FirstChildElement(); area_tag != NULL; area_tag = area_tag->NextSiblingElement()) {
			Area* area = new Area();
			area->setName(area_tag->Attribute("name"));
			area->m_id = area_tag->IntAttribute("id");

			auto shape_tag = area_tag->FirstChildElement("shape");
			if (shape_tag) {
				int point_count = shape_tag->IntAttribute("point_count");
				sf::ConvexShape* shape = new sf::ConvexShape(point_count);
				for (auto point_tag = shape_tag->FirstChildElement(); point_tag != NULL; point_tag = point_tag->NextSiblingElement()) {
					assert(point_tag->IntAttribute("index") < point_count);
					shape->setPoint(
						point_tag->IntAttribute("index"),
						sf::Vector2f(point_tag->FloatAttribute("x"), point_tag->FloatAttribute("y"))
					);
				}
				area->setShape(shape);
			}
			area_map[area->m_id] = area;
		}
	}

	void AssetsReader::readSprites(std::map<int, Sprite*>& sprite_map, std::map<int, Texture*>* texture_map) {
		auto sprites = m_doc->FirstChildElement()->FirstChildElement("sprites");
		for (auto spr_tag = sprites->FirstChildElement(); spr_tag!= NULL; spr_tag = spr_tag->NextSiblingElement()) {
			Sprite* sprite = new Sprite();
			sprite->setName(spr_tag->Attribute("name"));
			sprite->m_id = spr_tag->IntAttribute("id");
			
			auto tex_tag = spr_tag->FirstChildElement("texture");
			if (tex_tag && texture_map != nullptr) {
				int id = tex_tag->IntAttribute("id");
				assert((*texture_map)[id] != NULL && "can't find texture for the sprite");
				sprite->setTexture( *(*texture_map)[id] );

				sf::IntRect rect;
				rect.left = spr_tag->FirstChildElement("texture_rect")->IntAttribute("left");
				rect.top = spr_tag->FirstChildElement("texture_rect")->IntAttribute("top");
				rect.width = spr_tag->FirstChildElement("texture_rect")->IntAttribute("width");
				rect.height = spr_tag->FirstChildElement("texture_rect")->IntAttribute("height");
				sprite->setTextureRect(rect);

				glm::ivec4 frames;
				frames.x = spr_tag->FirstChildElement("frames")->IntAttribute("x");
				frames.y = spr_tag->FirstChildElement("frames")->IntAttribute("y");
				frames.z = spr_tag->FirstChildElement("frames")->IntAttribute("offset_x");
				frames.w = spr_tag->FirstChildElement("frames")->IntAttribute("offset_y");
				int index = spr_tag->FirstChildElement("frames")->IntAttribute("index");
				sprite->setFrames(frames);
				sprite->setFrameIndex(index);
			}
			sprite_map[sprite->m_id] = sprite;
		}
	}

	void AssetsReader::readBackground(std::map<int, Background*>& bg_map, std::map<int, Texture*>* texture_map) {
		auto bgs = m_doc->FirstChildElement()->FirstChildElement("backgrounds");
		for (auto bg_tag = bgs->FirstChildElement(); bg_tag != NULL; bg_tag = bg_tag->NextSiblingElement()) {
			Background* bg = new Background();
			bg->setName(bg_tag->Attribute("name"));
			bg->m_id = bg_tag->IntAttribute("id");

			bg->setVisible(bg_tag->FirstChildElement("properties")->BoolAttribute("visible"));
			bg->setMoveSpeed(
				bg_tag->FirstChildElement("move_speed")->IntAttribute("x"),
				bg_tag->FirstChildElement("move_speed")->IntAttribute("y")
			);
			bg->setScale(
				bg_tag->FirstChildElement("scale")->FloatAttribute("x"),
				bg_tag->FirstChildElement("scale")->FloatAttribute("y")
			);

			auto tex_tag = bg_tag->FirstChildElement("texture");
			if (tex_tag) {
				if (texture_map != nullptr) {
					int id = tex_tag->IntAttribute("id");
					assert((*texture_map)[id] != NULL && "can't find texture for the sprite");
					bg->setTexture((*texture_map)[id]);
					bg->setRepeatd(bg_tag->FirstChildElement("properties")->BoolAttribute("repeat"));
					bg->setSmooth(bg_tag->FirstChildElement("properties")->BoolAttribute("smooth"));
				}
			}
			bg_map[bg->m_id] = bg;
		}
	}

	void AssetsReader::readAnimation(std::map<int, Animation*>& anim_map) {
		auto bgs = m_doc->FirstChildElement()->FirstChildElement("animations");
		for (auto anim_tag = bgs->FirstChildElement(); anim_tag != NULL; anim_tag = anim_tag->NextSiblingElement()) {
			Animation* anim = new Animation();
			anim->m_id = anim_tag->IntAttribute("id");
			anim->setName( anim_tag->Attribute("name") );

			auto prop_tag = anim_tag->FirstChildElement("properties");
			anim->setTimeLength( prop_tag->FloatAttribute("time_length") );
			anim->setLoop( prop_tag->BoolAttribute("loop") );
			anim->setReverse( prop_tag->BoolAttribute("reverse") );

			auto obj_tag = anim_tag->FirstChildElement("object");
			if (obj_tag) {
				anim->m_object_id = anim_tag->FirstChildElement("object")->IntAttribute("id");
			}

			auto begin_trans_tag = anim_tag->FirstChildElement("begin_transform");
			anim->m_begin_position = glm::fvec2(
				begin_trans_tag->FirstChildElement("position")->FloatAttribute("x"),
				begin_trans_tag->FirstChildElement("position")->FloatAttribute("y")
			);
			anim->m_begin_rotation = begin_trans_tag->FirstChildElement("rotation")->FloatAttribute("angle");
			anim->m_begin_scale = glm::fvec2(
				begin_trans_tag->FirstChildElement("scale")->FloatAttribute("x"),
				begin_trans_tag->FirstChildElement("scale")->FloatAttribute("y")
			);

			auto sprite_frame_track_tag = anim_tag->FirstChildElement("sprite_frame_track");
			if (sprite_frame_track_tag) {
				SpriteFrameTrack* sprite_frame_track = new SpriteFrameTrack();
				for (auto key_tag = sprite_frame_track_tag->FirstChildElement(); key_tag != NULL; key_tag = key_tag->NextSiblingElement()) {
					Track::Key key;
					key.time = key_tag->FloatAttribute("time");
					key.data.sprite_frame = key_tag->IntAttribute("frame");
					sprite_frame_track->addKey(key);
				}
				anim->setSpriteFrameTrack(sprite_frame_track);
			}

			auto position_track_tag = anim_tag->FirstChildElement("position_track");
			if (position_track_tag) {
				PositionTrack* position_track = new PositionTrack;
				for (auto key_tag = position_track_tag->FirstChildElement(); key_tag != NULL; key_tag = key_tag->NextSiblingElement()) {
					Track::Key key;
					key.time = key_tag->FloatAttribute("time");
					key.data.position.x = key_tag->IntAttribute("x");
					key.data.position.y = key_tag->IntAttribute("y");
					position_track->addKey(key);
				}
				anim->setPositionTrack(position_track);
			}

			auto rotation_track_tag = anim_tag->FirstChildElement("rotation_track");
			if (rotation_track_tag) {
				RotationTrack* rotation_track = new RotationTrack();
				for (auto key_tag = rotation_track_tag->FirstChildElement(); key_tag != NULL; key_tag = key_tag->NextSiblingElement()) {
					Track::Key key;
					key.time = key_tag->FloatAttribute("time");
					key.data.rotation = key_tag->FloatAttribute("angle");
					rotation_track->addKey(key);
				}
				anim->setRotationTrack(rotation_track);
			}

			auto scale_track_tag = anim_tag->FirstChildElement("scale_track");
			if (scale_track_tag) {
				ScaleTrack* scale_track = new ScaleTrack();
				for (auto key_tag = scale_track_tag->FirstChildElement(); key_tag != NULL; key_tag = key_tag->NextSiblingElement()) {
					Track::Key key;
					key.time = key_tag->FloatAttribute("time");
					key.data.scale.x = key_tag->IntAttribute("x");
					key.data.scale.y = key_tag->IntAttribute("y");
					scale_track->addKey(key);
				}
				anim->setScaleTrack(scale_track);
			}

			// TODO:
			anim_map[anim->m_id] = anim;
		}
	}
}