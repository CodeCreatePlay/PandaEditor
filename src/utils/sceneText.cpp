#include "sceneText.hpp"

const float SceneText::MARGIN = 0.1;
const float SceneText::SHADOW = 0.4;

/*
	SceneText is implemented using the `Pimpl idiom' technique
	where the `implementation' part is done within struct TextNodeProxy.
*/
struct SceneText::TextNodeProxy : public TextNode 
{
	TextNodeProxy(const std::string& name, SceneText::TextStyle style);
	TextNodeProxy(const TextNodeProxy& other);
   
	LVecBase2f m_scale;
	LVecBase2f m_pos;
	float m_roll;
	float m_wordwrap;
   
	virtual ~TextNodeProxy();
	void update_transform_mat();
};

SceneText::TextNodeProxy::TextNodeProxy(const std::string& name, SceneText::TextStyle style) : 
	TextNode(name),
    m_scale(0, 0),
    m_pos(0, 0),
    m_roll(0),
    m_wordwrap(0)
{
   // Set default parameters according to the selected style.
   LColorf fg     (0, 0, 0, 0);
   LColorf bg     (0, 0, 0, 0);
   LColorf shadow (0, 0, 0, 0);
   LColorf frame  (0, 0 ,0 ,0);
   TextNode::Alignment align(A_center);
   
   switch(style)
   {
	   case TS_plain:
        m_scale = LVecBase2f(0.07, 0.07);
        fg      = LColorf(0, 0, 0, 1);
        bg      = LColorf(0, 0, 0, 0);
        shadow  = LColorf(0, 0, 0, 0);
        frame   = LColorf(0, 0, 0 ,0);
        break;

      case TS_screen_title:
        m_scale = LVecBase2f(0.15, 0.15);
        fg      = LColorf(1, 0.2, 0.2, 1);
        bg      = LColorf(0, 0, 0, 0);
        shadow  = LColorf(0, 0, 0, 1);
        frame   = LColorf(0, 0, 0 ,0);
        break;

      case TS_screen_prompt:
        m_scale = LVecBase2f(0.1, 0.1);
        fg      = LColorf(1, 1, 0, 1);
        bg      = LColorf(0, 0, 0, 0);
        shadow  = LColorf(0, 0, 0, 1);
        frame   = LColorf(0, 0, 0 ,0);
        break;

      case TS_name_confirm:
        m_scale = LVecBase2f(0.1, 0.1);
        fg      = LColorf(0, 1, 0, 1);
        bg      = LColorf(0, 0, 0, 0);
        shadow  = LColorf(0, 0, 0, 0);
        frame   = LColorf(0, 0, 0 ,0);
        break;

      case TS_black_on_white:
        m_scale = LVecBase2f(0.1, 0.1);
        fg      = LColorf(0, 0, 0, 1);
        bg      = LColorf(1, 1, 1, 1);
        shadow  = LColorf(0, 0, 0, 0);
        frame   = LColorf(0, 0, 0 ,0);
        break;

      default:
        nout << "DESIGN ERROR: missing a TextStyle case." << std::endl;
        return;
   }

	set_text_color(fg);
	set_align(align);

	if(bg[3] != 0)
	{
	  // If we have a background color, create a card.
	  set_card_color(bg);
	  set_card_as_margin(MARGIN, MARGIN, MARGIN, MARGIN);
	}

	if(shadow[3] != 0)
	{
	  // If we have a shadow color, create a shadow.
	  // Can't use the *shadow interface because it might be a VBase4.
	  // textNode.setShadowColor(*shadow)
	  set_shadow_color(shadow);
	  set_shadow(SHADOW, SHADOW);
	}

	if(frame[3] != 0)
	{
	  // If we have a frame color, create a frame.
	  set_frame_color(frame);
	  set_frame_as_margin(MARGIN, MARGIN, MARGIN, MARGIN);
	}

	update_transform_mat();
}

SceneText::TextNodeProxy::TextNodeProxy(const TextNodeProxy& other) : 
	TextNode(other),
	m_scale(other.m_scale),
	m_pos(other.m_pos),
	m_roll(other.m_roll),
	m_wordwrap(other.m_wordwrap)
{
   // empty
}

SceneText::TextNodeProxy::~TextNodeProxy()
{
   // Empty
}

void SceneText::TextNodeProxy::update_transform_mat()
{
	LMatrix4f mat =
		LMatrix4f::scale_mat(m_scale.get_x(), 1, m_scale.get_y()) *
		LMatrix4f::rotate_mat(m_roll, LVecBase3f(0, -1, 0)) *
		LMatrix4f::translate_mat(m_pos.get_x(), 0, m_pos.get_y());
	set_transform(mat);
}


SceneText::SceneText(const std::string& name, TextStyle style) : 
	m_textNode(new TextNodeProxy(name, style))
{
	// Ok, now update the node.
	NodePath::operator=(NodePath(m_textNode));
}

SceneText::SceneText(const SceneText& other) :
	m_textNode(new TextNodeProxy(*other.m_textNode))
{
	duplicate_parenting(other);
}

SceneText::~SceneText()
{
	cleanup();
}

SceneText& SceneText::operator=(const SceneText& other)
{
	SceneText(other).swap(*this);
	duplicate_parenting(other);
	return *this;
}

void SceneText::swap(SceneText& other)
{
	PT(TextNodeProxy) p = m_textNode;
	m_textNode = other.m_textNode;
	other.m_textNode = p;
}

void SceneText::duplicate_parenting(const SceneText& other)
{
	if(other.get_parent().is_empty())
	{
	  NodePath::operator=(NodePath(m_textNode));
	}
	else
	{
	  NodePath::operator=(other.get_parent().attach_new_node(m_textNode, other.get_sort()));
	}
}

void SceneText::cleanup()
{
	m_textNode = NULL;
	remove_node();
}

void SceneText::clear_text()
{
	m_textNode->clear_text();
}

void SceneText::append_text(const std::string& text)
{
	m_textNode->append_text(text);
}

NodePath SceneText::generate() const
{
	if(get_parent().is_empty())
	  return NodePath(m_textNode->generate());
	else
	  return get_parent().attach_new_node(m_textNode->generate(), get_sort());
}

void SceneText::set_font(TextFont* fontPtr)
{
	m_textNode->set_font(fontPtr);
}

void SceneText::set_text(const std::string& text)
{
	m_textNode->set_text(text);
}

void SceneText::set_decal(bool decal)
{
	m_textNode->set_card_decal(decal);
}

void SceneText::set_x(float x)
{
	set_pos(LVecBase2f(x, m_textNode->m_pos.get_y()));
}

void SceneText::set_y(float y)
{
	set_pos(LVecBase2f(m_textNode->m_pos.get_x(), y));
}

void SceneText::set_pos(float x, float y)
{
	set_pos(LVecBase2f(x, y));
}

void SceneText::set_pos(const LVecBase2f& pos)
{
	m_textNode->m_pos = pos;
	m_textNode->update_transform_mat();
}

void SceneText::set_scale(float scale)
{
	set_scale(LVecBase2f(scale, scale));
}

void SceneText::set_scale(float scaleX, float scaleY)
{
	set_scale(LVecBase2f(scaleX, scaleY));
}

void SceneText::set_scale(const LVecBase2f& scale)
{
	m_textNode->m_scale = scale;
	m_textNode->update_transform_mat();
}

void SceneText::set_roll(float roll)
{
	m_textNode->m_roll = roll;
	m_textNode->update_transform_mat();
}

void SceneText::set_wordwrap(float wordwrap)
{
	m_textNode->m_wordwrap = wordwrap;
	if(wordwrap != 0)
	{
	  m_textNode->set_wordwrap(wordwrap);
	}
	else
	{
	  m_textNode->clear_wordwrap();
	}
}

void SceneText::set_fg(const LColorf& fg)
{
	m_textNode->set_text_color(fg);
}

void SceneText::set_bg(const LColorf& bg)
{
	if(bg[3] > 0)
	{
	  // If we have a background color, create a card.
	  m_textNode->set_card_color(bg);
	  m_textNode->set_card_as_margin(MARGIN, MARGIN, MARGIN, MARGIN);
	}
	else
	{
	  // Otherwise, remove the card.
	  m_textNode->clear_card();
	}
}

void SceneText::set_shadow(const LColorf& shadow)
{
	if(shadow[3] > 0)
	{
	  // If we have a shadow color, create a shadow.
	  m_textNode->set_shadow_color(shadow);
	  m_textNode->set_shadow(SHADOW, SHADOW);
	}
	else
	{
	  // Otherwise, remove the shadow.
	  m_textNode->clear_shadow();
	}
}

void SceneText::set_shadow_offset(const LVecBase2f& offset)
{
	m_textNode->set_shadow(offset);
}

void SceneText::set_frame(const LColorf& frame)
{
	if(frame[3] > 0)
	{
	  // If we have a frame color, create a frame.
	  m_textNode->set_frame_color(frame);
	  m_textNode->set_frame_as_margin(MARGIN, MARGIN, MARGIN, MARGIN);
	}
	else
	{
	  // Otherwise, remove the frame.
	  m_textNode->clear_frame();
	}
}

void SceneText::set_align(TextNode::Alignment align)
{
	return m_textNode->set_align(align);
}

void SceneText::set_draw_order(int drawOrder)
{
	m_textNode->set_bin("fixed");
	m_textNode->set_draw_order(drawOrder);
}

TextFont* SceneText::get_font() const
{
	return m_textNode->get_font();
}

std::string SceneText::get_text() const
{
	return m_textNode->get_text();
}

bool SceneText::get_decal() const
{
	return m_textNode->get_card_decal();
}

const LVecBase2f& SceneText::get_pos() const
{
	return m_textNode->m_pos;
}

const LVecBase2f& SceneText::get_scale() const
{
	return m_textNode->m_scale;
}

float SceneText::get_roll() const
{
	return m_textNode->m_roll;
}

float SceneText::get_wordwrap() const
{
	return m_textNode->m_wordwrap;
}
