#ifndef ONSCREENTEXT_H
#define ONSCREENTEXT_H

#include <nodePath.h>
#include <textNode.h>

class SceneText : public NodePath {
public:
   enum TextStyle {
	   TS_plain = 1,
	   TS_screen_title,
	   TS_screen_prompt,
	   TS_name_confirm,
	   TS_black_on_white
    };

    SceneText(const std::string& name, TextStyle style = TS_plain);
    SceneText(const SceneText& other);
    ~SceneText();

	SceneText& operator=(const SceneText& other);

	void cleanup();
  	void clear_text();
	void append_text(const std::string& text);
	NodePath generate() const;
   
	void set_font(TextFont* font);
	void set_text(const std::string& text);
	void set_decal(bool decal);
   
	void set_x(float x);
	void set_y(float y);
	void set_pos(float x, float y);
	void set_pos(const LVecBase2f& pos);
	
	void set_scale(float scale);
	void set_scale(float scaleX, float scaleY);
	void set_scale(const LVecBase2f& scale);
	
	void set_roll(float roll);
	void set_wordwrap(float wordwarp);
   
	void set_fg(const LColorf& fg);
	void set_bg(const LColorf& bg);
	void set_shadow(const LColorf& shadow);
	void set_shadow_offset(const LVecBase2f& offset);
	
	void set_frame(const LColorf& frame);

	void set_align(TextNode::Alignment align);
	void set_draw_order(int drawOrder);
   
	TextFont* get_font() const;
	std::string get_text() const;
	bool get_decal() const;

	const LVecBase2f& get_pos() const;
	const LVecBase2f& get_scale() const;

	float get_roll() const;
	float get_wordwrap() const;

private:
   static const float MARGIN;
   static const float SHADOW;
   
   void swap(SceneText& other);
   void duplicate_parenting(const SceneText& other);

   struct TextNodeProxy;
   PT(TextNodeProxy) m_textNode;
};

#endif /* ONSCREENTEXT_H_ */
