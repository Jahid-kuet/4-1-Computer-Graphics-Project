import os
import sys
from pptx import Presentation
from pptx.util import Inches, Pt
from pptx.dml.color import RGBColor
from pptx.enum.text import PP_ALIGN, MSO_ANCHOR
from pptx.enum.shapes import MSO_SHAPE

sys.stdout.reconfigure(encoding='utf-8')

# Color Palette matching user's provided theme
TEAL_PRIMARY   = RGBColor(0x20, 0x79, 0x68)  # #207968 Deep Teal / Green accent
TEAL_MINT_BG   = RGBColor(0xE8, 0xF5, 0xF1)  # #E8F5F1 Light Mint badge background
DARK_TITLE     = RGBColor(0x11, 0x18, 0x27)  # #111827 Dark Navy / Near Black
DARK_BODY      = RGBColor(0x37, 0x41, 0x51)  # #374151 Slate Charcoal
MUTED_TEXT     = RGBColor(0x6B, 0x72, 0x80)  # #6B7280 Muted Gray
BORDER_LIGHT   = RGBColor(0xDC, 0xE7, 0xE4)  # #DCE7E4 Soft Gray-Teal border
WHITE          = RGBColor(0xFF, 0xFF, 0xFF)  # #FFFFFF

# Badge Accent Colors for Outline
BLUE_ACCENT    = RGBColor(0x25, 0x63, 0xEB)  # #2563EB
TEAL_ACCENT    = RGBColor(0x0D, 0x94, 0x88)  # #0D9488
ORANGE_ACCENT  = RGBColor(0xEA, 0x58, 0x0C)  # #EA580C
PINK_ACCENT    = RGBColor(0xE1, 0x1D, 0x48)  # #E11D48
PURPLE_ACCENT  = RGBColor(0x93, 0x33, 0xEA)  # #9333EA
CYAN_ACCENT    = RGBColor(0x02, 0x84, 0xC7)  # #0284C7

def create_deck():
    prs = Presentation()
    # 16:9 Widescreen dimensions
    prs.slide_width  = Inches(13.333)
    prs.slide_height = Inches(7.5)
    blank_layout = prs.slide_layouts[6] # Blank slide

    def add_top_accent_bar(slide):
        # Top teal border stripe spanning entire width
        bar = slide.shapes.add_shape(MSO_SHAPE.RECTANGLE, Inches(0), Inches(0), Inches(13.333), Inches(0.09))
        bar.fill.solid()
        bar.fill.fore_color.rgb = TEAL_PRIMARY
        bar.line.fill.background()

    def add_slide_header(slide, title_text, category_badge="CSE 4102: Computer Graphics Lab"):
        add_top_accent_bar(slide)
        
        # Pill container on top left
        badge = slide.shapes.add_shape(MSO_SHAPE.ROUNDED_RECTANGLE, Inches(0.8), Inches(0.35), Inches(4.2), Inches(0.65))
        badge.fill.solid()
        badge.fill.fore_color.rgb = TEAL_MINT_BG
        badge.line.color.rgb = TEAL_PRIMARY
        badge.line.width = Pt(1.2)
        tf = badge.text_frame
        tf.word_wrap = True
        tf.vertical_anchor = MSO_ANCHOR.MIDDLE
        p = tf.paragraphs[0]
        p.alignment = PP_ALIGN.CENTER
        p.text = title_text
        p.font.name = 'Arial'
        p.font.size = Pt(16)
        p.font.bold = True
        p.font.color.rgb = DARK_TITLE

        # Small department tag on top right
        tag_box = slide.shapes.add_textbox(Inches(7.5), Inches(0.35), Inches(5.0), Inches(0.65))
        tf_tag = tag_box.text_frame
        tf_tag.vertical_anchor = MSO_ANCHOR.MIDDLE
        p_tag = tf_tag.paragraphs[0]
        p_tag.alignment = PP_ALIGN.RIGHT
        p_tag.text = category_badge
        p_tag.font.name = 'Calibri'
        p_tag.font.size = Pt(11)
        p_tag.font.bold = True
        p_tag.font.color.rgb = TEAL_PRIMARY

    def add_footer(slide, page_num, total_pages=10):
        # Footer date on bottom-left
        dt_box = slide.shapes.add_textbox(Inches(0.8), Inches(6.9), Inches(3.0), Inches(0.4))
        p_dt = dt_box.text_frame.paragraphs[0]
        p_dt.text = "06/10/2026"
        p_dt.font.name = 'Calibri'
        p_dt.font.size = Pt(10)
        p_dt.font.bold = True
        p_dt.font.color.rgb = TEAL_PRIMARY

        # Footer page number on bottom-right
        pg_box = slide.shapes.add_textbox(Inches(10.5), Inches(6.9), Inches(2.0), Inches(0.4))
        p_pg = pg_box.text_frame.paragraphs[0]
        p_pg.alignment = PP_ALIGN.RIGHT
        p_pg.text = f"{page_num:02d} / {total_pages:02d}"
        p_pg.font.name = 'Calibri'
        p_pg.font.size = Pt(10)
        p_pg.font.bold = True
        p_pg.font.color.rgb = TEAL_PRIMARY

    # =========================================================================
    # SLIDE 1: TITLE PAGE (Following user's Image 1 theme)
    # =========================================================================
    s1 = prs.slides.add_slide(blank_layout)
    add_top_accent_bar(s1)

    # KUET Logo on upper-left
    if os.path.exists("kuet_logo.png"):
        s1.shapes.add_picture("kuet_logo.png", Inches(1.3), Inches(0.55), width=Inches(1.2), height=Inches(1.2))

    # University Pill Badge
    univ_pill = s1.shapes.add_shape(MSO_SHAPE.ROUNDED_RECTANGLE, Inches(2.7), Inches(0.55), Inches(9.3), Inches(0.60))
    univ_pill.fill.solid()
    univ_pill.fill.fore_color.rgb = TEAL_MINT_BG
    univ_pill.line.color.rgb = BORDER_LIGHT
    univ_pill.line.width = Pt(1.0)
    tf_u = univ_pill.text_frame
    tf_u.vertical_anchor = MSO_ANCHOR.MIDDLE
    p_u = tf_u.paragraphs[0]
    p_u.alignment = PP_ALIGN.CENTER
    p_u.text = "KHULNA UNIVERSITY OF ENGINEERING AND TECHNOLOGY"
    p_u.font.name = 'Arial'
    p_u.font.size = Pt(15)
    p_u.font.bold = True
    p_u.font.color.rgb = DARK_TITLE

    # Department & Course Subtitle
    dept_box = s1.shapes.add_textbox(Inches(2.7), Inches(1.15), Inches(9.3), Inches(0.6))
    tf_d = dept_box.text_frame
    p_d1 = tf_d.paragraphs[0]
    p_d1.alignment = PP_ALIGN.CENTER
    p_d1.text = "Department of Computer Science and Engineering"
    p_d1.font.name = 'Calibri'
    p_d1.font.size = Pt(12)
    p_d1.font.color.rgb = MUTED_TEXT

    p_d2 = tf_d.add_paragraph()
    p_d2.alignment = PP_ALIGN.CENTER
    p_d2.text = "CSE 4102: Computer Graphics and Image Processing Laboratory"
    p_d2.font.name = 'Calibri'
    p_d2.font.size = Pt(12)
    p_d2.font.bold = True
    p_d2.font.color.rgb = TEAL_PRIMARY

    # Main Project Title
    title_box = s1.shapes.add_textbox(Inches(1.0), Inches(2.1), Inches(11.333), Inches(1.7))
    tf_t = title_box.text_frame
    tf_t.word_wrap = True
    p_t = tf_t.paragraphs[0]
    p_t.alignment = PP_ALIGN.CENTER
    p_t.text = "Village Gathering by the River: A Bangladeshi Rural Scene in 3D"
    p_t.font.name = 'Arial'
    p_t.font.size = Pt(28)
    p_t.font.bold = True
    p_t.font.color.rgb = DARK_TITLE

    p_sub = tf_t.add_paragraph()
    p_sub.alignment = PP_ALIGN.CENTER
    p_sub.text = "Procedural Modeling, Dynamic Animation, Dual Shading & Whitted Ray Tracing using OpenGL 3.3"
    p_sub.font.name = 'Calibri'
    p_sub.font.size = Pt(14)
    p_sub.font.color.rgb = MUTED_TEXT
    p_sub.space_before = Pt(8)

    # Presenter Card (Bottom Left)
    c_pres = s1.shapes.add_shape(MSO_SHAPE.ROUNDED_RECTANGLE, Inches(1.8), Inches(4.3), Inches(4.4), Inches(2.2))
    c_pres.fill.solid()
    c_pres.fill.fore_color.rgb = WHITE
    c_pres.line.color.rgb = BORDER_LIGHT
    c_pres.line.width = Pt(1.2)
    tf_cp = c_pres.text_frame
    tf_cp.word_wrap = True
    tf_cp.vertical_anchor = MSO_ANCHOR.MIDDLE

    p1 = tf_cp.paragraphs[0]
    p1.text = "PRESENTED BY"
    p1.font.name = 'Calibri'
    p1.font.size = Pt(9.5)
    p1.font.bold = True
    p1.font.color.rgb = MUTED_TEXT

    p2 = tf_cp.add_paragraph()
    p2.text = "Jahid Hasan"
    p2.font.name = 'Arial'
    p2.font.size = Pt(15)
    p2.font.bold = True
    p2.font.color.rgb = DARK_TITLE
    p2.space_before = Pt(2)

    p3 = tf_cp.add_paragraph()
    p3.text = "Roll: 2107064\nDepartment of CSE,\nKhulna University of Engineering and Technology"
    p3.font.name = 'Calibri'
    p3.font.size = Pt(11)
    p3.font.color.rgb = DARK_BODY
    p3.space_before = Pt(4)

    # Supervisor Card (Bottom Right)
    c_sup = s1.shapes.add_shape(MSO_SHAPE.ROUNDED_RECTANGLE, Inches(7.1), Inches(4.3), Inches(4.4), Inches(2.2))
    c_sup.fill.solid()
    c_sup.fill.fore_color.rgb = WHITE
    c_sup.line.color.rgb = BORDER_LIGHT
    c_sup.line.width = Pt(1.2)
    tf_cs = c_sup.text_frame
    tf_cs.word_wrap = True
    tf_cs.vertical_anchor = MSO_ANCHOR.MIDDLE

    p_s1 = tf_cs.paragraphs[0]
    p_s1.text = "SUPERVISED BY"
    p_s1.font.name = 'Calibri'
    p_s1.font.size = Pt(9.5)
    p_s1.font.bold = True
    p_s1.font.color.rgb = MUTED_TEXT

    p_s2 = tf_cs.add_paragraph()
    p_s2.text = "Course Teachers"
    p_s2.font.name = 'Arial'
    p_s2.font.size = Pt(15)
    p_s2.font.bold = True
    p_s2.font.color.rgb = DARK_TITLE
    p_s2.space_before = Pt(2)

    p_s3 = tf_cs.add_paragraph()
    p_s3.text = "Department of Computer Science and Engineering,\nKhulna University of Engineering and Technology,\nKhulna-9203, Bangladesh"
    p_s3.font.name = 'Calibri'
    p_s3.font.size = Pt(11)
    p_s3.font.color.rgb = DARK_BODY
    p_s3.space_before = Pt(4)

    add_footer(s1, 1)

    # =========================================================================
    # SLIDE 2: OUTLINE (Following user's Image 2 theme)
    # =========================================================================
    s2 = prs.slides.add_slide(blank_layout)
    add_slide_header(s2, "Presentation Outline")

    # 6 Section Badges / Cards in 2 Columns x 3 Rows
    outline_items = [
        ("01", BLUE_ACCENT,   "Project Motivation & Scope Analysis", "Procedural philosophy, syllabus requirements & 100% manual code geometry"),
        ("02", TEAL_ACCENT,   "Proposal vs. Implementation Matrix",  "Initial targeted deliverables vs comprehensive extended achievements"),
        ("03", ORANGE_ACCENT, "Procedural Rural Architecture",       "Chouchala homesteads, updated semi-pukka mosque & Euclidean spatial layout"),
        ("04", PINK_ACCENT,   "River Kinematics & Bézier Curves",    "Trigonometric wave displacements, traditional boats & Terracotta Surahi"),
        ("05", PURPLE_ACCENT, "Hierarchical Motion & Vehicles",      "Bullock cart kinematics, Halchas plowing & tubewell groundwater pump"),
        ("06", CYAN_ACCENT,   "Dual Shading & Whitted Ray Tracing",  "Gouraud vs Phong comparison, physical attenuation & ray-traced shadows")
    ]

    col_w = Inches(5.6)
    row_h = Inches(1.5)
    start_x = [Inches(0.8), Inches(6.9)]
    start_y = [Inches(1.4), Inches(3.2), Inches(5.0)]

    for idx, (num, col_acc, title, desc) in enumerate(outline_items):
        cx = start_x[idx % 2]
        cy = start_y[idx // 2]

        card = s2.shapes.add_shape(MSO_SHAPE.ROUNDED_RECTANGLE, cx, cy, col_w, row_h)
        card.fill.solid()
        card.fill.fore_color.rgb = WHITE
        card.line.color.rgb = BORDER_LIGHT
        card.line.width = Pt(1.0)

        # Number pill badge
        nb = s2.shapes.add_shape(MSO_SHAPE.ROUNDED_RECTANGLE, cx + Inches(0.2), cy + Inches(0.25), Inches(0.9), Inches(0.55))
        nb.fill.solid()
        nb.fill.fore_color.rgb = WHITE
        nb.line.color.rgb = col_acc
        nb.line.width = Pt(2.0)
        tf_nb = nb.text_frame
        tf_nb.vertical_anchor = MSO_ANCHOR.MIDDLE
        p_nb = tf_nb.paragraphs[0]
        p_nb.alignment = PP_ALIGN.CENTER
        p_nb.text = num
        p_nb.font.name = 'Arial'
        p_nb.font.size = Pt(16)
        p_nb.font.bold = True
        p_nb.font.color.rgb = col_acc

        # Text block
        tb = s2.shapes.add_textbox(cx + Inches(1.25), cy + Inches(0.15), Inches(4.15), Inches(1.2))
        tf_txt = tb.text_frame
        tf_txt.word_wrap = True
        p_t = tf_txt.paragraphs[0]
        p_t.text = title
        p_t.font.name = 'Arial'
        p_t.font.size = Pt(13.5)
        p_t.font.bold = True
        p_t.font.color.rgb = DARK_TITLE

        p_d = tf_txt.add_paragraph()
        p_d.text = desc
        p_d.font.name = 'Calibri'
        p_d.font.size = Pt(10.5)
        p_d.font.color.rgb = MUTED_TEXT
        p_d.space_before = Pt(3)

    add_footer(s2, 2)

    # =========================================================================
    # SLIDE 3: PROPOSALS VS IMPLEMENTATION (Targeted vs Done)
    # =========================================================================
    s3 = prs.slides.add_slide(blank_layout)
    add_slide_header(s3, "Targeted Proposals vs. Delivered Systems")

    # Card 1: Initial Proposal Scope (Left)
    card_prop = s3.shapes.add_shape(MSO_SHAPE.ROUNDED_RECTANGLE, Inches(0.8), Inches(1.3), Inches(5.6), Inches(5.3))
    card_prop.fill.solid()
    card_prop.fill.fore_color.rgb = WHITE
    card_prop.line.color.rgb = RGBColor(0xEA, 0x58, 0x0C)
    card_prop.line.width = Pt(1.5)

    badge_p = s3.shapes.add_shape(MSO_SHAPE.ROUNDED_RECTANGLE, Inches(1.1), Inches(1.5), Inches(5.0), Inches(0.55))
    badge_p.fill.solid()
    badge_p.fill.fore_color.rgb = RGBColor(0xFF, 0xED, 0xD5)
    badge_p.line.fill.background()
    p_bp = badge_p.text_frame.paragraphs[0]
    p_bp.alignment = PP_ALIGN.CENTER
    p_bp.text = "TARGETED IN INITIAL PROPOSAL"
    p_bp.font.name = 'Arial'
    p_bp.font.size = Pt(12)
    p_bp.font.bold = True
    p_bp.font.color.rgb = RGBColor(0x9A, 0x34, 0x12)

    tb_prop = s3.shapes.add_textbox(Inches(1.1), Inches(2.2), Inches(5.0), Inches(4.2))
    tf_p = tb_prop.text_frame
    tf_p.word_wrap = True
    prop_points = [
        "1-2 Generic Rural Cottages: Basic mud walls and pitched roofs.",
        "Simple Subdivided River: Single sine-wave vertex displacement.",
        "1 Basic Rowboat: Solitary boatman rowing along river bank.",
        "Courtyard Gathering: Charpai bed, hand fan, storytelling elders.",
        "Baseline Night Sky: Single directional moonlight & blinking fireflies.",
        "Baseline Phong Lighting: Basic ambient + diffuse + specular model."
    ]
    for i, pt in enumerate(prop_points):
        p = tf_p.paragraphs[0] if i == 0 else tf_p.add_paragraph()
        p.text = "• " + pt
        p.font.name = 'Calibri'
        p.font.size = Pt(11)
        p.font.color.rgb = DARK_BODY
        if i > 0: p.space_before = Pt(8)

    # Card 2: Delivered Implementation (Right)
    card_del = s3.shapes.add_shape(MSO_SHAPE.ROUNDED_RECTANGLE, Inches(6.9), Inches(1.3), Inches(5.6), Inches(5.3))
    card_del.fill.solid()
    card_del.fill.fore_color.rgb = WHITE
    card_del.line.color.rgb = TEAL_PRIMARY
    card_del.line.width = Pt(1.5)

    badge_d = s3.shapes.add_shape(MSO_SHAPE.ROUNDED_RECTANGLE, Inches(7.2), Inches(1.5), Inches(5.0), Inches(0.55))
    badge_d.fill.solid()
    badge_d.fill.fore_color.rgb = TEAL_MINT_BG
    badge_d.line.fill.background()
    p_bd = badge_d.text_frame.paragraphs[0]
    p_bd.alignment = PP_ALIGN.CENTER
    p_bd.text = "COMPLETED & EXTENDED IN FINAL PROJECT"
    p_bd.font.name = 'Arial'
    p_bd.font.size = Pt(12)
    p_bd.font.bold = True
    p_bd.font.color.rgb = TEAL_PRIMARY

    tb_del = s3.shapes.add_textbox(Inches(7.2), Inches(2.2), Inches(5.0), Inches(4.2))
    tf_d = tb_del.text_frame
    tf_d.word_wrap = True
    del_points = [
        "10+ Chouchala Homesteads across 6 Historical Baris (mud plinths, 4-slope roofs, bamboo verandahs, smoking chimneys).",
        "Authentic Semi-Pukka Village Mosque: Concrete roof, turned brass dome finials, minaret Azaan horn loudspeaker & Ozukhana.",
        "Interactive Controllable Bullock Cart (Gorur Gari): 180° wheel turns (Key 'G') & manual driving (WASD).",
        "Agricultural Plowing (Halchas) & Tubewell (Key 'P'): Working oxen plow team and groundwater pumping physics.",
        "Parametric Cubic Bézier Surface of Revolution (Bonus): Terracotta Surahi with exact analytic normals (Key 'V').",
        "Dual Shading Engine (Key 'M'): Real-time Gouraud & Phong toggling.",
        "Real-Time Whitted Ray Tracing (Bonus, Key 'Y'): Primary rays, analytic intersections, shadow rays & mirror reflections."
    ]
    for i, pt in enumerate(del_points):
        p = tf_d.paragraphs[0] if i == 0 else tf_d.add_paragraph()
        p.text = "✓ " + pt
        p.font.name = 'Calibri'
        p.font.size = Pt(10.5)
        p.font.color.rgb = DARK_BODY
        if i > 0: p.space_before = Pt(6)

    add_footer(s3, 3)

    # =========================================================================
    # SLIDE 4: PROCEDURAL ARCHITECTURE & VILLAGE SETTLEMENT
    # =========================================================================
    s4 = prs.slides.add_slide(blank_layout)
    add_slide_header(s4, "Procedural Rural Architecture & Settlement Planning")

    # Image Left: Updated Mosque
    if os.path.exists("object_images/23_village_mosque.png"):
        s4.shapes.add_picture("object_images/23_village_mosque.png", Inches(0.8), Inches(1.3), width=Inches(3.8))
    # Image Middle: Chouchala House
    if os.path.exists("object_images/01_house_chouchala.png"):
        s4.shapes.add_picture("object_images/01_house_chouchala.png", Inches(4.8), Inches(1.3), width=Inches(3.8))

    # Right Card: Technical Highlights
    c_info4 = s4.shapes.add_shape(MSO_SHAPE.ROUNDED_RECTANGLE, Inches(8.8), Inches(1.3), Inches(3.7), Inches(5.3))
    c_info4.fill.solid()
    c_info4.fill.fore_color.rgb = WHITE
    c_info4.line.color.rgb = BORDER_LIGHT
    c_info4.line.width = Pt(1.0)
    tf_i4 = c_info4.text_frame
    tf_i4.word_wrap = True

    p = tf_i4.paragraphs[0]
    p.text = "ARCHITECTURAL MODELING"
    p.font.name = 'Arial'
    p.font.size = Pt(12)
    p.font.bold = True
    p.font.color.rgb = TEAL_PRIMARY

    bullets4 = [
        "100% Procedural Code: Composed from mathematical cubes, cylinders, cones, and parametric sweeps without external 3D files.",
        "Chouchala Four-Pitch Roof: Sloped trapezoids and triangular hip panels with ridge beam and bamboo rafters.",
        "Updated Rural Mosque: Plastered corrugated tin walls, concrete roof, turned brass dome finials, minaret Azaan horn loudspeaker & Ozukhana.",
        "Euclidean Spatial Planning: All homesteads adhere to minimum distance constraints: sqrt((Δx)² + (Δz)²) >= 4.8 m, preventing overlap.",
        "Normal Matrix Orthogonality: Computed as transpose(inverse(mat3(Model))) to prevent normal skewing under non-uniform scaling."
    ]
    for b in bullets4:
        pb = tf_i4.add_paragraph()
        pb.text = "• " + b
        pb.font.name = 'Calibri'
        pb.font.size = Pt(10.5)
        pb.font.color.rgb = DARK_BODY
        pb.space_before = Pt(8)

    # Bottom Landscape Image
    if os.path.exists("screenshot_shifted_house.png"):
        s4.shapes.add_picture("screenshot_shifted_house.png", Inches(0.8), Inches(4.1), width=Inches(7.8), height=Inches(2.5))

    add_footer(s4, 4)

    # =========================================================================
    # SLIDE 5: RIVER KINEMATICS, BOATS & PARAMETRIC BÉZIER GEOMETRY
    # =========================================================================
    s5 = prs.slides.add_slide(blank_layout)
    add_slide_header(s5, "River Kinematics & Parametric Bézier Surface")

    # Image Left: River Screenshot
    if os.path.exists("screenshot_river.png"):
        s5.shapes.add_picture("screenshot_river.png", Inches(0.8), Inches(1.3), width=Inches(5.6))

    # Image Top Right: Bézier Terracotta Surahi
    if os.path.exists("object_images/29_bezier_terracotta_vase.png"):
        s5.shapes.add_picture("object_images/29_bezier_terracotta_vase.png", Inches(6.6), Inches(1.3), width=Inches(2.8))

    # Image Bottom Right: Dingi Boat
    if os.path.exists("object_images/04_dingi_boat.png"):
        s5.shapes.add_picture("object_images/04_dingi_boat.png", Inches(9.6), Inches(1.3), width=Inches(2.9))

    # Bottom Card: Mathematical Derivations
    c_math5 = s5.shapes.add_shape(MSO_SHAPE.ROUNDED_RECTANGLE, Inches(0.8), Inches(4.5), Inches(11.7), Inches(2.1))
    c_math5.fill.solid()
    c_math5.fill.fore_color.rgb = WHITE
    c_math5.line.color.rgb = BORDER_LIGHT
    c_math5.line.width = Pt(1.0)
    tf_m5 = c_math5.text_frame
    tf_m5.word_wrap = True

    p = tf_m5.paragraphs[0]
    p.text = "MATHEMATICAL FORMULATIONS (RIVER WAVE & BÉZIER SURFACE)"
    p.font.name = 'Arial'
    p.font.size = Pt(12)
    p.font.bold = True
    p.font.color.rgb = TEAL_PRIMARY

    p_eq1 = tf_m5.add_paragraph()
    p_eq1.text = "1. River Wave Kinematics:  y(x, z, t) = y₀ + A₁ sin(k₁x + k₂z − ω₁t) + A₂ cos(k₃x − k₄z − ω₂t)  [Generates dynamic specular highlights]"
    p_eq1.font.name = 'Cambria Math'
    p_eq1.font.size = Pt(10.5)
    p_eq1.font.bold = True
    p_eq1.font.color.rgb = DARK_BODY
    p_eq1.space_before = Pt(4)

    p_eq2 = tf_m5.add_paragraph()
    p_eq2.text = "2. Cubic Bézier Profile Curve:  B(u) = (1−u)³ P₀ + 3u(1−u)² P₁ + 3u²(1−u) P₂ + u³ P₃,   u ∈ [0, 1]"
    p_eq2.font.name = 'Cambria Math'
    p_eq2.font.size = Pt(10.5)
    p_eq2.font.bold = True
    p_eq2.font.color.rgb = DARK_BODY
    p_eq2.space_before = Pt(3)

    p_eq3 = tf_m5.add_paragraph()
    p_eq3.text = "3. Surface of Revolution & Analytic Normal:  S(u, v) = [r(u) cos v, y(u), r(u) sin v]ᵀ,   n(u, v) = (T_u × T_v) / ||T_u × T_v||  (Key 'V' View)"
    p_eq3.font.name = 'Cambria Math'
    p_eq3.font.size = Pt(10.5)
    p_eq3.font.bold = True
    p_eq3.font.color.rgb = DARK_BODY
    p_eq3.space_before = Pt(3)

    add_footer(s5, 5)

    # =========================================================================
    # SLIDE 6: ARTICULATED CHARACTERS, VEHICLES & INTERACTIVE MECHANICS
    # =========================================================================
    s6 = prs.slides.add_slide(blank_layout)
    add_slide_header(s6, "Hierarchical Articulation, Vehicles & Rural Life")

    # 4 Visual Item Thumbnails Across Top
    imgs_s6 = [
        ("object_images/31_bullock_cart_gorur_gari.png", "Bullock Cart (Key 'R'/'G')"),
        ("screenshot_shifted_cowshed.png", "Thatched Cow Shed & Cattle"),
        ("object_images/33_fisherman_hunting_fish.png", "Fisherman & Cast Net (Key 'F')"),
        ("object_images/07_charpai_bed.png", "Woven Charpai & Storytelling")
    ]
    card_w6 = Inches(2.7)
    for i, (path, label) in enumerate(imgs_s6):
        x = Inches(0.8 + i * 2.95)
        c = s6.shapes.add_shape(MSO_SHAPE.ROUNDED_RECTANGLE, x, Inches(1.3), card_w6, Inches(3.0))
        c.fill.solid()
        c.fill.fore_color.rgb = WHITE
        c.line.color.rgb = BORDER_LIGHT
        c.line.width = Pt(1.0)
        if os.path.exists(path):
            s6.shapes.add_picture(path, x + Inches(0.1), Inches(1.4), width=Inches(2.5), height=Inches(2.2))
        lbl_box = s6.shapes.add_textbox(x, Inches(3.7), card_w6, Inches(0.5))
        p_l = lbl_box.text_frame.paragraphs[0]
        p_l.alignment = PP_ALIGN.CENTER
        p_l.text = label
        p_l.font.name = 'Arial'
        p_l.font.size = Pt(10)
        p_l.font.bold = True
        p_l.font.color.rgb = DARK_TITLE

    # Bottom Summary Card: Articulation Physics
    c_bot6 = s6.shapes.add_shape(MSO_SHAPE.ROUNDED_RECTANGLE, Inches(0.8), Inches(4.5), Inches(11.7), Inches(2.1))
    c_bot6.fill.solid()
    c_bot6.fill.fore_color.rgb = WHITE
    c_bot6.line.color.rgb = BORDER_LIGHT
    c_bot6.line.width = Pt(1.0)
    tf_b6 = c_bot6.text_frame
    tf_b6.word_wrap = True

    p = tf_b6.paragraphs[0]
    p.text = "HIERARCHICAL SCENE GRAPH & INTERACTIVE MECHANICS"
    p.font.name = 'Arial'
    p.font.size = Pt(12)
    p.font.bold = True
    p.font.color.rgb = TEAL_PRIMARY

    bullets6 = [
        "Parent-Child Matrix Chains: M_child = M_parent · M_joint(θ) · M_offset guarantees joint connectivity across character limbs.",
        "Bullock Cart Rolling Dynamics: Wheel rotation without slip satisfies θ_wheel(t) = θ₀ + (1/R_wheel) ∫ v_cart(t) dt. Key 'G' rotates wheels 180° and translates cart.",
        "Agricultural Plowing (Halchas): Two humped oxen harnessed to wooden Langol plow driven by farmer in lungi.",
        "Tubewell Pumping Kinematics: Fulcrum handle reciprocating motion θ_handle(t) = A_pump sin(ω_pump t) streams groundwater into clay Kolshi (Key 'P')."
    ]
    for b in bullets6:
        pb = tf_b6.add_paragraph()
        pb.text = "• " + b
        pb.font.name = 'Calibri'
        pb.font.size = Pt(10.5)
        pb.font.color.rgb = DARK_BODY
        pb.space_before = Pt(3)

    add_footer(s6, 6)

    # =========================================================================
    # SLIDE 7: ADVANCED ILLUMINATION & DUAL SHADING (GOURAUD VS PHONG)
    # =========================================================================
    s7 = prs.slides.add_slide(blank_layout)
    add_slide_header(s7, "Illumination Models & Dual Shading Architecture")

    # Left: Shading Comparison Card
    c_shd = s7.shapes.add_shape(MSO_SHAPE.ROUNDED_RECTANGLE, Inches(0.8), Inches(1.3), Inches(5.6), Inches(5.3))
    c_shd.fill.solid()
    c_shd.fill.fore_color.rgb = WHITE
    c_shd.line.color.rgb = TEAL_PRIMARY
    c_shd.line.width = Pt(1.5)
    tf_s = c_shd.text_frame
    tf_s.word_wrap = True

    p = tf_s.paragraphs[0]
    p.text = "GOURAUD VS. PHONG SHADING (KEY 'M')"
    p.font.name = 'Arial'
    p.font.size = Pt(13)
    p.font.bold = True
    p.font.color.rgb = TEAL_PRIMARY

    shd_text = [
        "Gouraud Shading (Per-Vertex):",
        "  • Lighting (Ambient + Diffuse + Specular) evaluated at each polygon VERTEX in Vertex Shader.",
        "  • Hardware rasterizer linearly interpolates lighting intensities (color) across fragments.",
        "  • Highly optimized execution; skips per-pixel pow() and vector normalizations.",
        "Phong Shading (Per-Fragment):",
        "  • Surface normal vectors are interpolated across triangle fragments.",
        "  • Blinn-Phong equation evaluated at every individual pixel in Fragment Shader.",
        "  • Delivers precise, crisp specular highlights on curved silhouettes.",
        "Live Toggle: Press Key 'M' to switch instantaneously between Gouraud and Phong shading!"
    ]
    for t in shd_text:
        pt = tf_s.add_paragraph()
        pt.text = t
        pt.font.name = 'Calibri'
        pt.font.size = Pt(10.5)
        pt.font.color.rgb = DARK_BODY
        if "Shading" in t or "Live" in t:
            pt.font.bold = True
            pt.space_before = Pt(6)
        else:
            pt.space_before = Pt(2)

    # Right: Multi-Light & Attenuation Card
    c_light = s7.shapes.add_shape(MSO_SHAPE.ROUNDED_RECTANGLE, Inches(6.9), Inches(1.3), Inches(5.6), Inches(5.3))
    c_light.fill.solid()
    c_light.fill.fore_color.rgb = WHITE
    c_light.line.color.rgb = BORDER_LIGHT
    c_light.line.width = Pt(1.0)
    tf_l = c_light.text_frame
    tf_l.word_wrap = True

    p = tf_l.paragraphs[0]
    p.text = "MULTI-LIGHT ATTENUATION & DAY/NIGHT"
    p.font.name = 'Arial'
    p.font.size = Pt(13)
    p.font.bold = True
    p.font.color.rgb = TEAL_PRIMARY

    light_text = [
        "Blinn-Phong Equation:  I = I_a + I_d + I_s + I_e",
        "  • Diffuse:  I_d = k_d · L_d · max(n · l, 0)",
        "  • Specular:  I_s = k_s · L_s · [max(n · h, 0)]^α,   h = (l + v) / ||l + v||",
        "6 Positional Point Lights:",
        "  • Courtyard Hariken, Moored Boat, Cruising Boat, Mosque Lamp, Clay Stove Fire, Ghat Lantern.",
        "Physical Inverse-Square Attenuation:",
        "  • Att(d) = [1 − (d/d_max)²]² / (k_c + k_l d + k_q d²)",
        "Hard Light vs Soft Light (Key 'U'):",
        "  • Soft Light: Organic wrap diffuse (0.75 diff + 0.25 n.y).",
        "  • Hard Light: Low ambient, sharp terminator, pow(diff, 1.35).",
        "Emissive Sources: Moon, Sun, Lantern flames (bypass external lighting)."
    ]
    for t in light_text:
        pt = tf_l.add_paragraph()
        pt.text = t
        pt.font.name = 'Calibri'
        pt.font.size = Pt(10.5)
        pt.font.color.rgb = DARK_BODY
        if "Equation" in t or "Lights" in t or "Attenuation" in t or "Hard" in t:
            pt.font.bold = True
            pt.space_before = Pt(6)
        else:
            pt.space_before = Pt(2)

    add_footer(s7, 7)

    # =========================================================================
    # SLIDE 8: REAL-TIME WHITTED RAY TRACING ENGINE (BONUS SHOWCASE)
    # =========================================================================
    s8 = prs.slides.add_slide(blank_layout)
    add_slide_header(s8, "Real-Time Whitted Ray Tracing Engine (Bonus)")

    # Left: Ray Tracing Architecture
    c_rt = s8.shapes.add_shape(MSO_SHAPE.ROUNDED_RECTANGLE, Inches(0.8), Inches(1.3), Inches(6.0), Inches(5.3))
    c_rt.fill.solid()
    c_rt.fill.fore_color.rgb = WHITE
    c_rt.line.color.rgb = TEAL_PRIMARY
    c_rt.line.width = Pt(1.5)
    tf_rt = c_rt.text_frame
    tf_rt.word_wrap = True

    p = tf_rt.paragraphs[0]
    p.text = "GLSL 330 RECURSIVE RAY TRACING ENGINE (KEY 'Y')"
    p.font.name = 'Arial'
    p.font.size = Pt(13)
    p.font.bold = True
    p.font.color.rgb = TEAL_PRIMARY

    rt_points = [
        "Whitted-Style Recursive Architecture: Implemented in hardware-accelerated GLSL computing analytic intersections without pre-rasterized geometry.",
        "Primary Ray Casting: Rays R(t) = O + t D are generated from perspective camera through screen pixel raster coordinates.",
        "Analytic Geometric Intersections:",
        "  • Ray-Sphere: Solves quadratic ||O + tD − C||² = r² via discriminant Δ = b² − 4ac.",
        "  • Ray-Box (AABB): Kay-Kajiya slab method evaluating entry/exit intervals t_near = max(min(t_x1, t_x2), min(t_y1, t_y2), min(t_z1, t_z2)).",
        "Direct Occlusion Shadow Rays: Secondary shadow rays cast toward light sources evaluate binary visibility S ∈ {0, 1}, rendering sharp shadows.",
        "Specular Mirror Reflections: Multi-bounce reflection rays D_reflect = D − 2(D · N)N simulated on water and metallic brass surfaces."
    ]
    for pt in rt_points:
        p_pt = tf_rt.add_paragraph()
        p_pt.text = "• " + pt
        p_pt.font.name = 'Calibri'
        p_pt.font.size = Pt(10.5)
        p_pt.font.color.rgb = DARK_BODY
        p_pt.space_before = Pt(6)

    # Right: Daytime & Sun Visual Card
    c_vis8 = s8.shapes.add_shape(MSO_SHAPE.ROUNDED_RECTANGLE, Inches(7.1), Inches(1.3), Inches(5.4), Inches(5.3))
    c_vis8.fill.solid()
    c_vis8.fill.fore_color.rgb = WHITE
    c_vis8.line.color.rgb = BORDER_LIGHT
    c_vis8.line.width = Pt(1.0)

    if os.path.exists("screenshot_day_sun.png"):
        s8.shapes.add_picture("screenshot_day_sun.png", Inches(7.3), Inches(1.5), width=Inches(5.0), height=Inches(2.7))

    tb_vis8 = s8.shapes.add_textbox(Inches(7.3), Inches(4.3), Inches(5.0), Inches(2.2))
    tf_v8 = tb_vis8.text_frame
    tf_v8.word_wrap = True
    p = tf_v8.paragraphs[0]
    p.text = "BONUS CURRICULUM HIGHLIGHTS"
    p.font.name = 'Arial'
    p.font.size = Pt(12)
    p.font.bold = True
    p.font.color.rgb = TEAL_PRIMARY

    v8_bullets = [
        "Whitted Ray Tracing toggled via Key 'Y' with interactive mouse camera orbit.",
        "Parametric Cubic Bézier Surface of Revolution inspected via Key 'V'.",
        "Multi-Mode Textures (Key 'X'): Solid -> Procedural GLSL -> GPU Texture Maps."
    ]
    for b in v8_bullets:
        pb = tf_v8.add_paragraph()
        pb.text = "✓ " + b
        pb.font.name = 'Calibri'
        pb.font.size = Pt(10.5)
        pb.font.color.rgb = DARK_BODY
        pb.space_before = Pt(4)

    add_footer(s8, 8)

    # =========================================================================
    # SLIDE 9: 2-MINUTE DEMONSTRATION VIDEO & INTERACTIVE CONTROLS
    # =========================================================================
    s9 = prs.slides.add_slide(blank_layout)
    add_slide_header(s9, "Motion, Interactivity & Demonstration Video Guide")

    # Left: Video Player Mockup Card
    c_vid = s9.shapes.add_shape(MSO_SHAPE.ROUNDED_RECTANGLE, Inches(0.8), Inches(1.3), Inches(5.8), Inches(5.3))
    c_vid.fill.solid()
    c_vid.fill.fore_color.rgb = DARK_TITLE
    c_vid.line.fill.background()

    # Video Thumbnail Background
    if os.path.exists("screenshot_day_sun.png"):
        s9.shapes.add_picture("screenshot_day_sun.png", Inches(0.9), Inches(1.4), width=Inches(5.6), height=Inches(3.2))

    # Play Button Overlay
    play_btn = s9.shapes.add_shape(MSO_SHAPE.OVAL, Inches(3.2), Inches(2.5), Inches(1.0), Inches(1.0))
    play_btn.fill.solid()
    play_btn.fill.fore_color.rgb = TEAL_PRIMARY
    play_btn.line.color.rgb = WHITE
    play_btn.line.width = Pt(2.0)
    p_play = play_btn.text_frame.paragraphs[0]
    p_play.alignment = PP_ALIGN.CENTER
    p_play.text = "▶"
    p_play.font.size = Pt(22)
    p_play.font.bold = True
    p_play.font.color.rgb = WHITE

    # Video Breakdown Box inside card
    tb_vb = s9.shapes.add_textbox(Inches(1.0), Inches(4.7), Inches(5.4), Inches(1.8))
    tf_vb = tb_vb.text_frame
    tf_vb.word_wrap = True
    p = tf_vb.paragraphs[0]
    p.text = "2-MINUTE LIVE DEMONSTRATION TIMELINE"
    p.font.name = 'Arial'
    p.font.size = Pt(11)
    p.font.bold = True
    p.font.color.rgb = TEAL_MINT_BG

    timeline = [
        "0:00 - 0:30 : Automated Panoramic Cinematic Tour (SPACE) & Day/Night Celestial Switch (Key 'L').",
        "0:30 - 1:00 : Interactive Bullock Cart Driving (WASD/Key 'G') & Tubewell Groundwater Pumping (Key 'P').",
        "1:00 - 1:30 : Dingi Boat Rowing (Key 'N'), River Fisherman Net Cast (Key 'F') & Wave Kinematics.",
        "1:30 - 2:00 : Real-Time Gouraud vs Phong Shading Toggle (Key 'M') & Whitted Ray Tracing (Key 'Y')."
    ]
    for t in timeline:
        pt = tf_vb.add_paragraph()
        pt.text = t
        pt.font.name = 'Calibri'
        pt.font.size = Pt(9.5)
        pt.font.color.rgb = WHITE
        pt.space_before = Pt(3)

    # Right: Complete Interactive Keyboard Matrix
    c_keys = s9.shapes.add_shape(MSO_SHAPE.ROUNDED_RECTANGLE, Inches(6.9), Inches(1.3), Inches(5.6), Inches(5.3))
    c_keys.fill.solid()
    c_keys.fill.fore_color.rgb = WHITE
    c_keys.line.color.rgb = BORDER_LIGHT
    c_keys.line.width = Pt(1.0)
    tf_k = c_keys.text_frame
    tf_k.word_wrap = True

    p = tf_k.paragraphs[0]
    p.text = "LIVE INTERACTION KEYBOARD MATRIX"
    p.font.name = 'Arial'
    p.font.size = Pt(13)
    p.font.bold = True
    p.font.color.rgb = TEAL_PRIMARY

    keybindings = [
        ("Mouse Drag & Wheel", "Spherical Orbit Camera & Smooth Dolly Zoom"),
        ("W / A / S / D", "Free 3D Fly Navigation through village"),
        ("SPACE", "Toggle Automated Cinematic Fly-Through Tour"),
        ("Key 'M'", "Toggle Shading: Phong (Per-Fragment) <-> Gouraud (Per-Vertex)"),
        ("Key 'Y'", "Toggle Real-Time Whitted Recursive Ray Tracing"),
        ("Key 'V'", "Inspect Parametric Cubic Bézier Terracotta Surahi"),
        ("Key 'R' / 'G'", "Drive Bullock Cart / Rotate Wheels 180° Step"),
        ("Key 'N'", "Steer & Row Traditional Dingi Nouka Boat"),
        ("Key 'P'", "Pump Tubewell Handle (Water flows into Kolshi)"),
        ("Key 'L' / 'J' / 'O'", "Day/Night Cycle, Directional Light & 6 Point Lights"),
        ("Key '1' - '0'", "10 Landmark Homestay Bookmark Camera Views")
    ]
    for k, desc in keybindings:
        pk = tf_k.add_paragraph()
        pk.text = f"[{k}] : {desc}"
        pk.font.name = 'Calibri'
        pk.font.size = Pt(9.5)
        pk.font.color.rgb = DARK_BODY
        pk.space_before = Pt(3)

    add_footer(s9, 9)

    # =========================================================================
    # SLIDE 10: CONCLUSION & THANK YOU
    # =========================================================================
    s10 = prs.slides.add_slide(blank_layout)
    add_top_accent_bar(s10)

    # KUET Logo Centered
    if os.path.exists("kuet_logo.png"):
        s10.shapes.add_picture("kuet_logo.png", Inches(5.9), Inches(0.8), width=Inches(1.5), height=Inches(1.5))

    # Thank You Title
    t_box10 = s10.shapes.add_textbox(Inches(1.0), Inches(2.4), Inches(11.333), Inches(1.0))
    p_th = t_box10.text_frame.paragraphs[0]
    p_th.alignment = PP_ALIGN.CENTER
    p_th.text = "Thank You!"
    p_th.font.name = 'Arial'
    p_th.font.size = Pt(40)
    p_th.font.bold = True
    p_th.font.color.rgb = TEAL_PRIMARY

    # Project Summary Card
    c_sum10 = s10.shapes.add_shape(MSO_SHAPE.ROUNDED_RECTANGLE, Inches(2.2), Inches(3.6), Inches(8.9), Inches(2.8))
    c_sum10.fill.solid()
    c_sum10.fill.fore_color.rgb = WHITE
    c_sum10.line.color.rgb = BORDER_LIGHT
    c_sum10.line.width = Pt(1.2)
    tf_s10 = c_sum10.text_frame
    tf_s10.word_wrap = True

    p = tf_s10.paragraphs[0]
    p.alignment = PP_ALIGN.CENTER
    p.text = "PROJECT HIGHLIGHTS SUMMARY"
    p.font.name = 'Arial'
    p.font.size = Pt(13)
    p.font.bold = True
    p.font.color.rgb = DARK_TITLE

    sum_points = [
        "100% Procedurally Modeled: Zero external 3D asset loaders (.obj/.fbx); purely mathematical C++20 / OpenGL 3.3.",
        "Complete Syllabus Adherence: Transformations, Normal Matrix, Hierarchical Animation, Multi-Light Attenuation.",
        "Dual Shading Models: Live runtime comparison between Per-Vertex Gouraud Shading and Per-Fragment Phong Shading (Key 'M').",
        "Bonus Achievements: Real-Time Whitted Ray Tracing Engine (Key 'Y') & Parametric Cubic Bézier Surface of Revolution (Key 'V').",
        "Interactive Mechanics: Controllable Bullock Cart driving, Dingi boat rowing, Tubewell pumping & 10 preset camera views."
    ]
    for sp in sum_points:
        psp = tf_s10.add_paragraph()
        psp.text = "✓ " + sp
        psp.font.name = 'Calibri'
        psp.font.size = Pt(11)
        psp.font.color.rgb = DARK_BODY
        psp.space_before = Pt(4)

    # Questions & Answers Invite
    qa_box = s10.shapes.add_textbox(Inches(1.0), Inches(6.5), Inches(11.333), Inches(0.5))
    p_qa = qa_box.text_frame.paragraphs[0]
    p_qa.alignment = PP_ALIGN.CENTER
    p_qa.text = "Questions & Discussions Welcome | Roll: 2107064 | Department of CSE, KUET"
    p_qa.font.name = 'Calibri'
    p_qa.font.size = Pt(12)
    p_qa.font.bold = True
    p_qa.font.color.rgb = MUTED_TEXT

    add_footer(s10, 10)

    # Save presentation
    out_pptx = "Village_Gathering_By_The_River_Presentation.pptx"
    prs.save(out_pptx)
    print(f"Presentation successfully created and saved to: {out_pptx}")

create_deck()
