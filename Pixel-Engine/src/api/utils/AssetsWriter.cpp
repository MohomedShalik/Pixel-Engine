#include "pch.h"
#include "AssetsWriter.h"

#include "tinyxml2.h"


namespace pe
{
	void AssetsWriter::addAsset(Asset* asset) {
		switch (asset->getType())
		{
		case Asset::Type::Texture:		addTexture(static_cast<Texture*>(asset)); break;
		case Asset::Type::Font:			addFont(static_cast<Font*>(asset)); break;
		case Asset::Type::Area:			addArea(static_cast<Area*>(asset)); break;
		case Asset::Type::Sprite:		addSprite(static_cast<Sprite*>(asset)); break;
		case Asset::Type::Background:	addBackground(static_cast<Background*>(asset)); break;
		case Asset::Type::Animation:	addAnimation(static_cast<Animation*>(asset)); break;
		case Asset::Type::Object:		addObject(static_cast<Object*>(asset)); break;
		case Asset::Type::Scene:		addScene(static_cast<Scene*>(asset)); break;
		default:						break;
		}
	}
	void AssetsWriter::addAsset(int id) {
		assert( pe::Assets::hasAsset(id) );
		addAsset( pe::Assets::s_assets[id] );
	}

	void AssetsWriter::addAssets() {
		for (auto asset : pe::Assets::s_assets) {
			addAsset(asset.second);
		}
	}
	void AssetsWriter::addAssets(std::map<int, Asset*>& asset_map) {
		for (auto asset : asset_map) {
			addAsset(asset.second);
		}
	}
	void AssetsWriter::addAssets(std::vector<int>& assets_vec) {
		for (int id : assets_vec) {
			addAsset(id);
		}
	}
	/// adding assets

	void AssetsWriter::save(const char* path) {
		m_doc->SaveFile(path); 
		m_doc->Clear();
		init();
	}

	AssetsWriter::AssetsWriter() {
		m_doc = new tinyxml2::XMLDocument();
		init();
	}

	void AssetsWriter::init()
	{
		auto assets = m_doc->NewElement("assets");
		m_doc->InsertFirstChild(assets);

		auto textures = m_doc->NewElement("textures");
		auto fonts = m_doc->NewElement("fonts");
		auto areas = m_doc->NewElement("areas");
		auto sprites = m_doc->NewElement("sprites");
		auto backgrounds = m_doc->NewElement("backgrounds");
		auto animations = m_doc->NewElement("animations");
		auto objects = m_doc->NewElement("objects");
		auto scenes = m_doc->NewElement("scenes");
		// tile map, ...

		assets->InsertEndChild(textures);
		assets->InsertEndChild(fonts);
		assets->InsertEndChild(areas);
		assets->InsertEndChild(sprites);
		assets->InsertEndChild(backgrounds);
		assets->InsertEndChild(animations);
		assets->InsertEndChild(objects);
		assets->InsertEndChild(scenes);
	}
	//*
	void AssetsWriter::_setPeproj( const struct _peproj& proj) {
		m_doc->Clear();

		auto root = m_doc->NewElement("Project");
		root->SetAttribute("title", proj.title.c_str());
		m_doc->InsertFirstChild(root);

		auto size_tag = m_doc->NewElement("window_size");
		root->InsertEndChild(size_tag);
		size_tag->SetAttribute("x",proj.window_size.x);
		size_tag->SetAttribute("y",proj.window_size.y);

		auto pref_tag = m_doc->NewElement("pref");
		root->InsertEndChild(pref_tag);
		pref_tag->SetAttribute("frame_rate", proj.frame_rate);
		pref_tag->SetAttribute("begin_scene_id", proj.begin_scene_id);
		pref_tag->SetAttribute("debug_mode", proj.is_debug_mode);
		pref_tag->SetAttribute("no_console", proj.no_console_window);
		
		auto assets_tag = m_doc->NewElement("assets");
		root->InsertEndChild(assets_tag);
		for (auto path : proj.assets_paths) {
			auto path_tag = m_doc->NewElement("path");
			assets_tag->InsertEndChild(path_tag);
			path_tag->SetText(path.c_str());
		}
		int id = proj.window_icon_texture_id;
		if ( id  >= 0 ) {
			auto window_icon_tag = m_doc->NewElement("window_icon");
			root->InsertEndChild(window_icon_tag);
			window_icon_tag->SetAttribute("texture_id", proj.window_icon_texture_id);
		}

		auto bg_color = m_doc->NewElement("bg_color");
		root->InsertEndChild(bg_color);
		bg_color->SetAttribute("r", proj.default_bg_color.r);
		bg_color->SetAttribute("g", proj.default_bg_color.g);
		bg_color->SetAttribute("b", proj.default_bg_color.b);
		bg_color->SetAttribute("a", proj.default_bg_color.a);
	}
	//*/

	////////////////////////////////////////////////////////////////////////////////////
	void AssetsWriter::addTexture(Texture* texture) {
		auto textures = m_doc->FirstChildElement()->FirstChildElement("textures");
		auto texture_tag = m_doc->NewElement("texture");
		textures->InsertEndChild(texture_tag);
		texture_tag->SetAttribute("name", texture->getName().c_str());
		texture_tag->SetAttribute("id", texture->getId());
		texture_tag->SetAttribute("smooth", texture->isSmooth());
		texture_tag->SetAttribute("repeat", texture->isRepeated());
		texture_tag->SetText(texture->getPath().c_str());
	}

	void AssetsWriter::addFont(Font* font) {
		auto fonts = m_doc->FirstChildElement()->FirstChildElement("fonts");
		auto font_tag = m_doc->NewElement("font");
		fonts->InsertEndChild(font_tag);
		font_tag->SetAttribute("name", font->getName().c_str());
		font_tag->SetAttribute("id", font->getId());
		font_tag->SetText(font->getPath().c_str());
	}

	void AssetsWriter::addArea(Area* area) {
		auto areas = m_doc->FirstChildElement()->FirstChildElement("areas");
		auto area_tag = m_doc->NewElement("area");
		areas->InsertEndChild(area_tag);
		area_tag->SetAttribute("name", area->getName().c_str());
		area_tag->SetAttribute("id", area->getId());

		if (area->hasShape()) {
			auto shape_tag = m_doc->NewElement("shape");
			area_tag->InsertEndChild(shape_tag);
			shape_tag->SetAttribute("point_count", (int)area->getShape().getPointCount());
			for (int i = 0; i < area->getShape().getPointCount(); i++) {
				auto point_tag = m_doc->NewElement("point");
				shape_tag->InsertEndChild(point_tag);
				point_tag->SetAttribute("index", i);
				point_tag->SetAttribute("x", area->getShape().getPoint(i).x);
				point_tag->SetAttribute("y", area->getShape().getPoint(i).y);
			}
		}
	}

	void AssetsWriter::addSprite(Sprite* sprite) {
		auto sprites = m_doc->FirstChildElement()->FirstChildElement("sprites");
		auto sprite_tag = m_doc->NewElement("sprite");
		sprites->InsertEndChild(sprite_tag);
		sprite_tag->SetAttribute("name", sprite->getName().c_str());
		sprite_tag->SetAttribute("id", sprite->getId());

		
		if (sprite->hasTexture()) {
			auto texture_tag = m_doc->NewElement("texture");
			sprite_tag->InsertEndChild(texture_tag);
			texture_tag->SetAttribute("id", sprite->getTexture().getId());

			auto texture_rect_tag = m_doc->NewElement("texture_rect");
			sprite_tag->InsertEndChild(texture_rect_tag);
			texture_rect_tag->SetAttribute("left", sprite->getTextureRect().left);
			texture_rect_tag->SetAttribute("top", sprite->getTextureRect().top);
			texture_rect_tag->SetAttribute("width", sprite->getTextureRect().width);
			texture_rect_tag->SetAttribute("height", sprite->getTextureRect().height);

			auto frames_tag = m_doc->NewElement("frames");
			sprite_tag->InsertEndChild(frames_tag);
			frames_tag->SetAttribute("x", std::get<0>(sprite->getFrames()).x);
			frames_tag->SetAttribute("y", std::get<0>(sprite->getFrames()).y);
			frames_tag->SetAttribute("offset_x", std::get<1>(sprite->getFrames()).x);
			frames_tag->SetAttribute("offset_y", std::get<1>(sprite->getFrames()).y);
			frames_tag->SetAttribute("index", sprite->getFrameIndex());
		}
	}


	void AssetsWriter::addBackground(Background* bg) {
		auto bgs = m_doc->FirstChildElement()->FirstChildElement("backgrounds");
		auto bg_tag = m_doc->NewElement("background");
		bgs->InsertEndChild(bg_tag);
		bg_tag->SetAttribute("name", bg->getName().c_str());
		bg_tag->SetAttribute("id", bg->getId());
		

		auto prop = m_doc->NewElement("properties");
		bg_tag->InsertEndChild(prop);
		prop->SetAttribute("visible", bg->isVisible());

		auto speed_tag = m_doc->NewElement("move_speed");
		bg_tag->InsertEndChild(speed_tag);
		speed_tag->SetAttribute("x", bg->getMoveSpeed().x);
		speed_tag->SetAttribute("y", bg->getMoveSpeed().y);

		auto scale_tag = m_doc->NewElement("scale");
		bg_tag->InsertEndChild(scale_tag);
		scale_tag->SetAttribute("x", bg->getScale().x);
		scale_tag->SetAttribute("y", bg->getScale().y);

		if (bg->hasTexture()) {
			auto texture_tag = m_doc->NewElement("texture");
			bg_tag->InsertEndChild(texture_tag);
			texture_tag->SetAttribute("id", bg->getTexture().getId());
		}
	}

	void AssetsWriter::addAnimation(Animation* anim) {
		auto anims = m_doc->FirstChildElement()->FirstChildElement("animations");
		auto anim_tag = m_doc->NewElement("animation");
		anims->InsertEndChild(anim_tag);
		anim_tag->SetAttribute("name", anim->getName().c_str());
		anim_tag->SetAttribute("id", anim->getId());

		auto prop = m_doc->NewElement("properties");
		anim_tag->InsertEndChild(prop);
		prop->SetAttribute("time_length", anim->getTimeLength());
		prop->SetAttribute("loop", anim->isLoop());
		prop->SetAttribute("reverse", anim->isReverse());

		if (anim->getObject()) {
			auto obj_tag = m_doc->NewElement("object");
			anim_tag->InsertEndChild(obj_tag);
			obj_tag->SetAttribute("id", anim->getObject()->getId());
		}

		auto begin_transform_tag = m_doc->NewElement("begin_transform");
		anim_tag->InsertEndChild(begin_transform_tag);
		auto begin_pos_tag = m_doc->NewElement("position");
		begin_transform_tag->InsertEndChild(begin_pos_tag);
		begin_pos_tag->SetAttribute("x", anim->_getBeginPosition().x);
		begin_pos_tag->SetAttribute("y", anim->_getBeginPosition().y);
		auto begin_rotation_tag = m_doc->NewElement("rotation");
		begin_transform_tag->InsertEndChild(begin_rotation_tag);
		begin_rotation_tag->SetAttribute("angle", anim->_getBeginRotation());
		auto begin_scale_tag = m_doc->NewElement("scale");
		begin_transform_tag->InsertEndChild(begin_scale_tag);
		begin_scale_tag->SetAttribute("x", anim->_getBeginScale().x);
		begin_scale_tag->SetAttribute("y", anim->_getBeginScale().y);
		
		if (anim->_getSpriteFrameTrack()) {
			auto sprite_frame_track_tag = m_doc->NewElement("sprite_frame_track");
			anim_tag->InsertEndChild(sprite_frame_track_tag);
			for (auto key : anim->_getSpriteFrameTrack()->getKeys()) {
				auto key_tag = m_doc->NewElement("key");
				sprite_frame_track_tag->InsertEndChild(key_tag);
				key_tag->SetAttribute("time", key.time);
				key_tag->SetAttribute("frame", key.data.sprite_frame);
			}
		}
		if (anim->_getPositionTrack()) {
			auto position_track_tag = m_doc->NewElement("position_track");
			anim_tag->InsertEndChild(position_track_tag);
			for (auto key : anim->_getPositionTrack()->getKeys()) {
				auto key_tag = m_doc->NewElement("key");
				position_track_tag->InsertEndChild(key_tag);
				key_tag->SetAttribute("time", key.time);
				key_tag->SetAttribute("x", key.data.position.x);
				key_tag->SetAttribute("y", key.data.position.y);
			}
		}
		if (anim->_getRotationTrack()) {
			auto rotation_track_tag = m_doc->NewElement("rotation_track");
			anim_tag->InsertEndChild(rotation_track_tag);
			for (auto key : anim->_getRotationTrack()->getKeys()) {
				auto key_tag = m_doc->NewElement("key");
				rotation_track_tag->InsertEndChild(key_tag);
				key_tag->SetAttribute("time", key.time);
				key_tag->SetAttribute("angle", key.data.rotation);
			}
		}
		if (anim->_getScaleTrack()) {
			auto scale_track_tag = m_doc->NewElement("scale_track");
			anim_tag->InsertEndChild(scale_track_tag);
			for (auto key : anim->_getScaleTrack()->getKeys()) {
				auto key_tag = m_doc->NewElement("key");
				scale_track_tag->InsertEndChild(key_tag);
				key_tag->SetAttribute("time", key.time);
				key_tag->SetAttribute("x", key.data.scale.x);
				key_tag->SetAttribute("y", key.data.scale.y);
			}
		}
	}

	void AssetsWriter::addObject(Object* obj) {
		auto objects = m_doc->FirstChildElement()->FirstChildElement("objects");
		auto obj_tag = m_doc->NewElement("object");
		objects->InsertEndChild(obj_tag);
		obj_tag->SetAttribute("class_name", obj->getClassName().c_str());
		obj_tag->SetAttribute("name", obj->getName().c_str());
		obj_tag->SetAttribute("id", obj->getId());


		auto prop = m_doc->NewElement("properties");
		obj_tag->InsertEndChild(prop);
		prop->SetAttribute("z_index", obj->getZIndex());
		prop->SetAttribute("visible", obj->isVisible());
		prop->SetAttribute("persistence", obj->isPersistence());

		auto transform_tag = m_doc->NewElement("transform");
		obj_tag->InsertEndChild(transform_tag);

		auto pos_tag = m_doc->NewElement("position");
		transform_tag->InsertEndChild(pos_tag);
		pos_tag->SetAttribute("x", obj->getPosition().x);
		pos_tag->SetAttribute("y", obj->getPosition().y);
		auto rot_tag = m_doc->NewElement("rotation");
		transform_tag->InsertEndChild(rot_tag);
		rot_tag->SetAttribute("angle", obj->getRotation() );
		auto scale_tag = m_doc->NewElement("scale");
		transform_tag->InsertEndChild(scale_tag);
		scale_tag->SetAttribute("x", obj->getScale().x);
		scale_tag->SetAttribute("y", obj->getScale().y);
		auto origin_tag = m_doc->NewElement("origin");
		transform_tag->InsertEndChild(origin_tag);
		origin_tag->SetAttribute( "x", obj->getOrigin().x );
		origin_tag->SetAttribute( "y", obj->getOrigin().y );

		if (obj->hasSprite()) {
			auto spr_tag = m_doc->NewElement("sprite");
			obj_tag->InsertEndChild(spr_tag);
			spr_tag->SetAttribute("id", obj->getSprite().getId());
		}
		if (obj->hasArea()) {
			auto area_tag = m_doc->NewElement("area");
			obj_tag->InsertEndChild(area_tag);
			area_tag->SetAttribute("id", obj->getArea().getId());
		}
		auto anims_tag = m_doc->NewElement("animations");
		obj_tag->InsertEndChild(anims_tag);
		for (auto anim : obj->getAnimations()) {
			auto anim_tag = m_doc->NewElement("animation");
			anims_tag->InsertEndChild(anim_tag);
			anim_tag->SetAttribute("id", anim.second->getId());
		}
	}

	void AssetsWriter::addScene(Scene* scene) {
		auto scenes = m_doc->FirstChildElement()->FirstChildElement("scenes");
		auto scn_tag = m_doc->NewElement("scene");
		scenes->InsertEndChild(scn_tag);
		scn_tag->SetAttribute("name", scene->getName().c_str());
		scn_tag->SetAttribute("id", scene->getId());

		auto window_size_tag = m_doc->NewElement("window_size");
		scn_tag->InsertEndChild(window_size_tag);
		window_size_tag->SetAttribute("width", scene->m_window_size.x);
		window_size_tag->SetAttribute("height", scene->m_window_size.y);

		if (scene->getBackground()) {
			auto bg_tag = m_doc->NewElement("background");
			scn_tag->InsertEndChild(bg_tag);
			bg_tag->SetAttribute("id", scene->getBackground()->getId() );
		}

		auto objs_tag = m_doc->NewElement("objects");
		scn_tag->InsertEndChild(objs_tag);
		for (Object* obj : scene->m_objects) {
			auto obj_tag = m_doc->NewElement("object");
			objs_tag->InsertEndChild(obj_tag);
			obj_tag->SetAttribute("id",obj->getId());
		}
	}
}