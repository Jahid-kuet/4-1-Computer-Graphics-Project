import os
import sys
import docx
from docx.shared import Inches, Pt, RGBColor
from docx.enum.text import WD_ALIGN_PARAGRAPH
from docx.enum.table import WD_TABLE_ALIGNMENT, WD_ALIGN_VERTICAL
from docx.oxml import OxmlElement, parse_xml
from docx.oxml.ns import nsdecls, qn

sys.stdout.reconfigure(encoding='utf-8')

def set_cell_background(cell, fill_hex):
    tcPr = cell._tc.get_or_add_tcPr()
    shd = parse_xml(f'<w:shd {nsdecls("w")} w:fill="{fill_hex}"/>')
    tcPr.append(shd)

def set_cell_margins(cell, top=100, bottom=100, left=150, right=150):
    tcPr = cell._tc.get_or_add_tcPr()
    tcMar = parse_xml(f'<w:tcMar {nsdecls("w")}><w:top w:w="{top}" w:type="dxa"/><w:bottom w:w="{bottom}" w:type="dxa"/><w:left w:w="{left}" w:type="dxa"/><w:right w:w="{right}" w:type="dxa"/></w:tcMar>')
    tcPr.append(tcMar)

def create_report():
    doc = docx.Document()

    # Page setup - A4 with 0.75 in margins
    for s in doc.sections:
        s.page_width = Inches(8.27)
        s.page_height = Inches(11.69)
        s.top_margin = Inches(0.75)
        s.bottom_margin = Inches(0.75)
        s.left_margin = Inches(0.75)
        s.right_margin = Inches(0.75)

    # Base font setup
    style_normal = doc.styles['Normal']
    font = style_normal.font
    font.name = 'Calibri'
    font.size = Pt(11)
    font.color.rgb = RGBColor(0x22, 0x22, 0x22)

    # Helper functions
    def add_title(text):
        p = doc.add_paragraph()
        p.alignment = WD_ALIGN_PARAGRAPH.CENTER
        p.paragraph_format.space_before = Pt(0)
        p.paragraph_format.space_after = Pt(4)
        run = p.add_run(text)
        run.font.name = 'Arial'
        run.font.size = Pt(20)
        run.bold = True
        run.font.color.rgb = RGBColor(0x11, 0x33, 0x66)
        return p

    def add_subtitle(text):
        p = doc.add_paragraph()
        p.alignment = WD_ALIGN_PARAGRAPH.CENTER
        p.paragraph_format.space_before = Pt(0)
        p.paragraph_format.space_after = Pt(18)
        run = p.add_run(text)
        run.font.name = 'Arial'
        run.font.size = Pt(13)
        run.font.color.rgb = RGBColor(0x55, 0x55, 0x55)
        return p

    def add_meta_info(lines):
        p = doc.add_paragraph()
        p.alignment = WD_ALIGN_PARAGRAPH.CENTER
        p.paragraph_format.space_before = Pt(0)
        p.paragraph_format.space_after = Pt(24)
        for line, is_bold, sz, color in lines:
            run = p.add_run(line + '\n')
            run.font.name = 'Calibri'
            run.font.size = Pt(sz)
            run.bold = is_bold
            run.font.color.rgb = color
        return p

    def add_h1(text):
        p = doc.add_paragraph()
        p.paragraph_format.space_before = Pt(18)
        p.paragraph_format.space_after = Pt(6)
        p.paragraph_format.keep_with_next = True
        run = p.add_run(text)
        run.font.name = 'Arial'
        run.font.size = Pt(15)
        run.bold = True
        run.font.color.rgb = RGBColor(0x0F, 0x38, 0x6E)
        return p

    def add_h2(text):
        p = doc.add_paragraph()
        p.paragraph_format.space_before = Pt(12)
        p.paragraph_format.space_after = Pt(4)
        p.paragraph_format.keep_with_next = True
        run = p.add_run(text)
        run.font.name = 'Arial'
        run.font.size = Pt(12.5)
        run.bold = True
        run.font.color.rgb = RGBColor(0x20, 0x4A, 0x87)
        return p

    def add_p(text):
        p = doc.add_paragraph()
        p.paragraph_format.space_before = Pt(0)
        p.paragraph_format.space_after = Pt(6)
        p.paragraph_format.line_spacing = 1.15
        run = p.add_run(text)
        run.font.size = Pt(10.5)
        return p

    def add_bullet(text):
        p = doc.add_paragraph(style='List Bullet')
        p.paragraph_format.space_before = Pt(0)
        p.paragraph_format.space_after = Pt(3)
        p.paragraph_format.line_spacing = 1.15
        run = p.add_run(text)
        run.font.size = Pt(10.5)
        return p

    def add_math(text):
        p = doc.add_paragraph()
        p.alignment = WD_ALIGN_PARAGRAPH.CENTER
        p.paragraph_format.space_before = Pt(4)
        p.paragraph_format.space_after = Pt(4)
        run = p.add_run(text)
        run.font.name = 'Cambria Math'
        run.font.size = Pt(11)
        run.bold = True
        run.font.color.rgb = RGBColor(0x1A, 0x2A, 0x3A)
        return p

    def add_image_safe(img_path, caption_text, width=Inches(5.8)):
        if os.path.exists(img_path):
            p = doc.add_paragraph()
            p.alignment = WD_ALIGN_PARAGRAPH.CENTER
            p.paragraph_format.space_before = Pt(8)
            p.paragraph_format.space_after = Pt(2)
            run = p.add_run()
            run.add_picture(img_path, width=width)
            
            cp = doc.add_paragraph()
            cp.alignment = WD_ALIGN_PARAGRAPH.CENTER
            cp.paragraph_format.space_before = Pt(2)
            cp.paragraph_format.space_after = Pt(10)
            crun = cp.add_run(caption_text)
            crun.font.name = 'Calibri'
            crun.font.size = Pt(9.5)
            crun.italic = True
            crun.font.color.rgb = RGBColor(0x55, 0x55, 0x55)
        else:
            print(f"Warning: Image not found: {img_path}")

    # ==================== COVER / HEADER ====================
    add_title("Khulna University of Engineering & Technology")
    add_subtitle("Department of Computer Science and Engineering\nCourse: CSE 4102 — Computer Graphics & Image Processing Laboratory")

    meta = [
        ("PROJECT REPORT", True, 16, RGBColor(0x0F, 0x38, 0x6E)),
        ("Village Gathering by the River: A Bangladeshi Rural Scene in 3D", True, 14, RGBColor(0x1B, 0x4D, 0x3E)),
        ("Procedurally Modeled, Animated & Real-Time Rendered using C++20 and OpenGL 3.3 Core Profile", False, 11, RGBColor(0x55, 0x55, 0x55)),
        ("Candidate Roll: 2107064", True, 11, RGBColor(0x22, 0x22, 0x22)),
        ("Date of Submission: October 2026", False, 10, RGBColor(0x66, 0x66, 0x66)),
    ]
    add_meta_info(meta)

    add_image_safe("screenshot_day_sun.png", "Figure 1: Full-plane overview of the Bangladeshi riverine village scene under daytime lighting.")

    # ==================== CHAPTER 01 ====================
    add_h1("01  Abstract & Project Objectives")
    add_h2("Abstract")
    add_p("This project presents a mathematically modeled, interactive, and animated 3D graphical simulation of a traditional Bangladeshi rural settlement situated beside a flowing river. Built from foundational principles using modern C++20 and the OpenGL 3.3 Core Profile graphics pipeline, the entire environment—comprising multi-roomed clay and corrugated-iron homesteads (Bari), a historic multi-domed village mosque, indigenous trees, traditional wooden boats (Dingi and large cargo sailboats), a functioning bullock cart (Gorur Gari), agricultural plowing (Halchas), livestock, and villagers—is constructed entirely through code without importing external 3D mesh files (.obj, .fbx, or .gltf).")
    add_p("The rendering system incorporates dual illumination architectures: a rasterized per-fragment Blinn-Phong shading engine featuring 6 physically attenuated point lights and directional moonlight/sunlight, alongside a newly implemented per-vertex Gouraud shading pipeline with instant runtime toggling. Furthermore, the application integrates an advanced real-time Whitted-style recursive Ray Tracing engine in GLSL 330, a parametric Cubic Bézier surface of revolution generator (demonstrated via an authentic Bengali Terracotta Surahi with analytic normals), dynamic trigonometric river wave kinematics, hierarchical articulated animation, and extensive user interactivity.")

    add_h2("Practical Goals")
    add_bullet("Procedural Scene Construction: Construct an extensive, culturally authentic rural environment entirely from fundamental geometric primitives and parametric sweeps without third-party asset loaders.")
    add_bullet("Core Graphics Transformations: Demonstrate rigorous 3D translation, rotation, scaling, and hierarchical parent-child transformation chains, with strict normal matrix orthogonality.")
    add_bullet("Dual Shading Implementations: Implement and contrast both Per-Fragment Blinn-Phong Shading and Per-Vertex Gouraud Shading with real-time switching.")
    add_bullet("Advanced Illumination: Simulate composite illumination comprising directional celestial sources (Moon / Sun), 6 positional point lights with physical quadratic distance attenuation, and switchable hard/soft lighting falloffs.")
    add_bullet("Ray Tracing Engine (Bonus Objective): Implement real-time Whitted ray tracing evaluating primary rays, analytic sphere/box intersections, occlusion shadow rays, and recursive specular reflection.")
    add_bullet("Parametric Surfaces & Curves (Bonus Objective): Construct smooth 3D surfaces of revolution from Cubic Bézier curves using Bernstein basis polynomials and analytical tangent-derived surface normals.")
    add_bullet("Comprehensive User Interaction: Provide multiple camera navigation modes (orbit, fly-through, landmark bookmarks), interactive vehicle driving, and physical mechanics controls.")

    add_h2("Proposed Features vs. Final Implementation Matrix")
    add_p("The initial project proposal focused on a nighttime courtyard gathering. In the completed project, every proposed feature was fully realized, and substantial high-difficulty extensions were added to fulfill and exceed the laboratory curriculum requirements:")

    # Table of Features
    table = doc.add_table(rows=1, cols=4)
    table.alignment = WD_TABLE_ALIGNMENT.CENTER
    hdr = table.rows[0].cells
    hdr[0].text = "Feature / Component"
    hdr[1].text = "Initial Proposal"
    hdr[2].text = "Final Implementation Status"
    hdr[3].text = "Verification & Controls"
    for cell in hdr:
        set_cell_background(cell, "0F386E")
        set_cell_margins(cell, 120, 120, 150, 150)
        p = cell.paragraphs[0]
        p.runs[0].font.color.rgb = RGBColor(0xFF, 0xFF, 0xFF)
        p.runs[0].font.bold = True
        p.runs[0].font.size = Pt(9.5)

    features_data = [
        ("Traditional Chouchala Houses", "1–2 generic cottages", "10+ architecturally accurate homesteads across 6 historical Baris (mud plinths, 4-slope roofs, bamboo posts, smoking chimneys)", "Key '1'–'4', '8' views"),
        ("River & Flowing Water", "Subdivided grid with sine displacement", "Multi-frequency sine/cosine wave kinematics with dynamic specular highlights and stepped ghat", "Key '2' view, Key 'F'"),
        ("Boats & Majhi", "1 rowboat with rowing Majhi", "3 traditional boats: Dingi Nouka with bamboo Chhoi, cargo sailboat with mast, passenger boat; interactive driving & rowing kinematics", "Key 'N' (Row/Drive), Key '2'"),
        ("Courtyard Gathering", "Charpai bed, hand fan, seated elders", "Authentic braided Charpai, cane Moras, hand fan (Haat Pakha), storytelling elders, children reading poems", "Key '1' view, default mode"),
        ("Fauna & Agricultural Work", "Hens and ducks", "Pecking hens, swimming ducks, resting humped cattle in thatched cow shed (Gowal Ghor), farmer & oxen plowing field (Halchas)", "Key '0' view, Key 'H' plow step"),
        ("Bullock Cart (Gorur Gari)", "Not in original proposal (Added)", "Fully articulated traditional bullock cart with spoked wooden wheels, curved canopy, oxen team, interactive steering & driving", "Key 'R' (Drive), Key 'G' (Turn)"),
        ("Historic Village Mosque", "Not in original proposal (Added)", "Multi-domed brick masonry mosque with central hemispherical dome, crescent finials, multi-tier minarets, arched Mehrab portal", "Key '7' view"),
        ("Traditional Tubewell", "Not in original proposal (Added)", "Cast-iron tubewell with reciprocating fulcrum handle, spout, terracotta Kolshi, interactive pumping animation", "Key 'P' (Pump water)"),
        ("Parametric Bézier Surface", "Not in original proposal (Bonus)", "Terracotta Surahi (মাটির সুরাহি) generated via Cubic Bézier curve surface of revolution with exact analytical normals", "Key 'V' view"),
        ("Gouraud & Phong Shading", "Phong baseline only", "Dual shading models: Per-Vertex Gouraud Shading and Per-Fragment Blinn-Phong Shading with dynamic switching", "Key 'M' toggle"),
        ("Whitted Ray Tracing", "Not in original proposal (Bonus)", "Real-time Whitted recursive ray tracing in GLSL 330: primary rays, analytic sphere/box intersections, shadow rays, mirror reflections", "Key 'Y' toggle"),
        ("Multi-Light System", "Single moonlight source", "Directional Moon/Sun light + 6 positional point lights with inverse-square attenuation + Hard/Soft light toggle", "Key 'J', 'O', 'U', 'L'"),
        ("Camera & Navigation", "Basic orbit camera", "Mouse orbit, wheel dolly zoom, free 3D fly (WASD), 10 homestay landmark bookmarks, automated cinematic fly-through tour", "Mouse, WASD, Key '1'–'0', SPACE"),
    ]

    for row_idx, data in enumerate(features_data):
        row = table.add_row()
        bg = "F4F6F9" if row_idx % 2 == 1 else "FFFFFF"
        for c_idx, text in enumerate(data):
            cell = row.cells[c_idx]
            cell.text = text
            set_cell_background(cell, bg)
            set_cell_margins(cell, 80, 80, 100, 100)
            p = cell.paragraphs[0]
            p.runs[0].font.size = Pt(9)
            if c_idx == 0:
                p.runs[0].font.bold = True

    # ==================== CHAPTER 02 ====================
    add_h1("02  Mathematical Coordinate Pipeline & Transformations")
    add_h2("World Coordinate Conventions")
    add_p("The 3D virtual environment is constructed in a right-handed Cartesian coordinate system:")
    add_bullet("X-axis (East-West): Defines the lateral village span. The river channel flows from North to South along the eastern boundary (X ≈ +2.0 to +16.0), while homestead courtyards extend westward (X ≈ −4.0 to −45.0).")
    add_bullet("Y-axis (Elevation): Defines vertical altitude. Ground level is established at Y = 0.0, the sunken riverbed rests at Y = −0.65, water surface flows at Y = 0.0, and roof peaks reach Y ≈ +3.8.")
    add_bullet("Z-axis (North-South): Defines depth along the river corridor (Z ≈ −50.0 in the upstream north to +50.0 in the downstream south).")

    add_h2("The OpenGL Transformation Pipeline")
    add_p("Every 3D vertex v_local defined in model space traverses six coordinate spaces before rasterization:")
    add_p("1. Model Space: Local canonical object geometry (e.g. unit cube, cylinder, or parametric patch).")
    add_p("2. World Space: Positioned into the global village scene via the Model Matrix M:")
    add_math("v_world = M · v_local = (T · R · S) · v_local")
    add_p("3. View Space (Camera Space): Oriented relative to the camera eye position via the View Matrix V:")
    add_math("v_view = V · v_world")
    add_p("4. Clip Space: Transformed through perspective foreshortening via the Projection Matrix P:")
    add_math("v_clip = P · v_view = P · V · M · v_local")
    add_p("5. Normalized Device Coordinates (NDC): Perspective division performed in hardware:")
    add_math("v_ndc = [ x_clip / w_clip,  y_clip / w_clip,  z_clip / w_clip ]^T ∈ [−1, 1]^3")
    add_p("6. Window / Viewport Coordinates: Mapped to screen pixel raster dimensions (1280 × 720).")

    add_h2("Mathematical Transformation Derivations")
    add_p("The composite model matrix M is derived from the product of elementary translation T, rotation R, and scaling S matrices:")
    add_math("T(t_x, t_y, t_z) = [ [1, 0, 0, t_x], [0, 1, 0, t_y], [0, 0, 1, t_z], [0, 0, 0, 1] ]")
    add_math("S(s_x, s_y, s_z) = [ [s_x, 0, 0, 0], [0, s_y, 0, 0], [0, 0, s_z, 0], [0, 0, 0, 1] ]")
    add_p("Rotations about coordinate axes by angle θ (Euler rotation matrices):")
    add_math("R_x(θ) = [ [1, 0, 0, 0], [0, cos θ, -sin θ, 0], [0, sin θ, cos θ, 0], [0, 0, 0, 1] ]")
    add_math("R_y(θ) = [ [cos θ, 0, sin θ, 0], [0, 1, 0, 0], [-sin θ, 0, cos θ, 0], [0, 0, 0, 1] ]")
    add_math("R_z(θ) = [ [cos θ, -sin θ, 0, 0], [sin θ, cos θ, 0, 0], [0, 0, 1, 0], [0, 0, 0, 1] ]")

    add_h2("Normal Matrix Derivation for Non-Uniform Scaling")
    add_p("Under non-uniform scaling (s_x ≠ s_y ≠ s_z), surface tangent vectors transform directly by the linear model matrix M_3×3, but surface normal vectors do not remain orthogonal to the surface if multiplied directly by M_3×3.")
    add_p("Proof: Let t be a surface tangent vector (n · t = 0). Under transformation, the transformed tangent is t' = M_3×3 · t. We seek a normal transformation matrix G such that the transformed normal n' = G · n remains orthogonal to t':")
    add_math("(n')^T · t' = 0  ==>  (G · n)^T · (M_3×3 · t) = 0")
    add_math("n^T · G^T · M_3×3 · t = 0")
    add_p("For this equality to hold for all tangent vectors where n^T · t = 0, we must have:")
    add_math("G^T · M_3×3 = I  ==>  G^T = (M_3×3)^(-1)  ==>  G = ((M_3×3)^(-1))^T")
    add_p("Therefore, the Normal Matrix implemented in our vertex shader is strictly computed as:")
    add_math("Normal_Matrix = transpose( inverse( mat3(Model) ) )")

    # ==================== CHAPTER 03 ====================
    add_h1("03  Procedural Architecture & Village Settlement Layout")
    add_h2("Traditional Chouchala House Design")
    add_p("Traditional rural Bengali domestic architecture features four-sloped roofs (Chouchala) engineered to withstand heavy monsoon rainfall. Each homestead is composed procedurally of distinct architectural components:")
    add_bullet("Mud Plinth (ভিটি): Raised rectangular earthen base (Y = 0.0 to 0.25) shielding living quarters from monsoon runoff.")
    add_bullet("Clay Walls: Structural enclosures formed from partitioned mud and clay cuboids with realistic earthy albedo.")
    add_bullet("Four-Pitched Roof (Chouchala): A central ridge beam with four sloping trapezoidal and triangular roof panels angled at 32°.")
    add_bullet("Verandah (বারান্দা): Open front portico supported by cylindrical bamboo posts with eaves overhang.")
    add_bullet("Kitchen Chimney: Earthen smoke vent with animated buoyant particle smoke puffs rising into the air.")

    add_image_safe("object_images/01_house_chouchala.png", "Figure 2: Procedurally generated traditional Chouchala homestead.", width=Inches(4.2))
    add_image_safe("screenshot_shifted_house.png", "Figure 3: Rural homestead with clay walls, veranda posts, and adjacent agricultural terrain.", width=Inches(5.6))

    add_h2("Settlement Layout & Collision-Free Spatial Planning")
    add_p("Rather than placing buildings randomly, the village is organized into historical family compounds (Baris) with a deterministic layout engine verifying minimum safety distances:")
    add_math("Distance(Bari_i, Bari_j) = sqrt( (x_i - x_j)^2 + (z_i - z_j)^2 ) >= d_min = 4.8 m")
    add_bullet("Moddho Bari (Central Homestead): Focal courtyard with elder gathering charpai, hand-pumped tubewell, and kitchen.")
    add_bullet("Uttar Bari (North Farmstead): Adjacent to the thatched cattle shed (Gowal Ghor) and storage.")
    add_bullet("Dokkhin Bari (South Agricultural Homestead): Bordering paddy fields and plowing grounds.")
    add_bullet("Poschim Bari (West Meadow Settlement): Situated along the western road corridor.")
    add_bullet("Nodi-Par Settlement: Riverside fisherman cottage directly accessing the mooring ghat.")

    add_h2("Traditional Village Mosque (টিনের ও পাকা গ্রামীণ মসজিদ)")
    add_p("In the north-west quadrant lies the village mosque, modeled after authentic rural Bangladeshi semi-pukka mosques:")
    add_bullet("Corrugated Tin & Plastered Enclosure: White-plastered walls reinforced with corrugated tin sheet siding, dark wooden framed entrance doors, and green cornices.")
    add_bullet("Flat Concrete Roof & Central Dome: Flat slab concrete roof (চ্যাপ্টা ছাদ) supporting a central dome crowned with a turned brass spindle finial (কলস / চূড়া without star or crescent).")
    add_bullet("Minarets & Azaan Horn Loudspeaker: Slender octagonal minaret tower equipped with a traditional Ahuja-style horn loudspeaker (চোঙা মাইক) for calling the faithful to prayer.")
    add_bullet("Dedicated Ablution Area (Ozukhana): Adjacent courtyard ablution platform equipped with traditional clay water pitchers (মাটির বদনা / কলসি).")
    add_bullet("Mehrab Arched Portal & Hanging Lantern: Semicircular entrance arch illuminated by an ambient hanging lantern (Point Light 3).")

    add_image_safe("object_images/23_village_mosque.png", "Figure 4: Updated procedural rural village mosque with corrugated tin walls, turned brass dome finial, and minaret Azaan horn loudspeaker.", width=Inches(4.5))
    add_image_safe("screenshot_shifted_mosque.png", "Figure 5: Rural village mosque situated in the northern coconut and bamboo grove with walking devout elder.", width=Inches(5.6))

    # ==================== CHAPTER 04 ====================
    add_h1("04  Parametric Curves & Cubic Bézier Surfaces (Bonus Feature)")
    add_h2("Mathematical Formulation of Cubic Bézier Curves")
    add_p("To fulfill the advanced curriculum requirement of parametric curve modeling, a 3D Surface of Revolution was developed based on a Cubic Bézier curve. A cubic Bézier curve is defined by four control points P_0, P_1, P_2, P_3 in R^2:")
    add_math("B(u) = sum_(i=0)^3 B_(i,3)(u) · P_i,   u in [0, 1]")
    add_p("where B_(i,3)(u) are the Bernstein basis polynomials:")
    add_math("B_(0,3)(u) = (1 - u)^3")
    add_math("B_(1,3)(u) = 3u(1 - u)^2")
    add_math("B_(2,3)(u) = 3u^2(1 - u)")
    add_math("B_(3,3)(u) = u^3")

    add_h2("Surface of Revolution Generation")
    add_p("A cross-sectional profile curve [r(u), y(u)] is revolved around the central vertical Y-axis by azimuthal angle v ∈ [0, 2π):")
    add_math("S(u, v) = [ r(u) cos v,   y(u),   r(u) sin v ]^T")

    add_h2("Analytic Normal Vector Derivation")
    add_p("Rather than approximating surface normals from flat triangle faces, our implementation evaluates exact analytical normals via partial derivatives of the parametric surface equation:")
    add_math("T_u = (del S) / (del u) = [ (dr/du) cos v,   (dy/du),   (dr/du) sin v ]^T")
    add_math("T_v = (del S) / (del v) = [ -r(u) sin v,   0,   r(u) cos v ]^T")
    add_p("The cross product T_u × T_v yields the orthogonal surface normal vector:")
    add_math("N(u, v) = T_u xx T_v = [ (dy/du) r(u) cos v,   -(dr/du) r(u),   (dy/du) r(u) sin v ]^T")
    add_math("n(u, v) = (N(u, v)) / (||N(u, v)||)")

    add_h2("Bengali Terracotta Surahi (মাটির সুরাহি)")
    add_p("This mathematical formulation was utilized to create a traditional Bengali Terracotta Surahi (an earthen pitcher used for cooling drinking water). The profile curve incorporates four Bézier segments modeling the flared lip, slender neck, bulbous body, and reinforced foot ring. Pressing Key 'V' activates a dedicated inspection camera focused on the vase.")

    add_image_safe("object_images/29_bezier_terracotta_vase.png", "Figure 6: Parametric Cubic Bézier Terracotta Surahi with analytic normals.", width=Inches(3.8))

    # ==================== CHAPTER 05 ====================
    add_h1("05  Water Kinematics, River Dynamics & Agricultural Terrain")
    add_h2("Meandering River Geometry")
    add_p("The river channel follows a natural meandering curve defined mathematically along the north-south Z-axis:")
    add_math("X_(center)(z) = X_0 + A_1 sin(k_1 z + phi_1) + A_2 cos(k_2 z)")
    add_p("where X_0 = 8.0 m, A_1 = 1.8 m, k_1 = 0.08 rad/m, and A_2 = 0.6 m.")

    add_h2("Dynamic Wave Surface Kinematics")
    add_p("The river surface mesh is subdivided into a high-density grid where vertex vertical displacement y(x, z, t) is modulated continuously over time t using a composite trigonometric wave function:")
    add_math("y(x, z, t) = y_0 + A_w1 sin(k_x1 x + k_z1 z - omega_1 t) + A_w2 cos(k_x2 x - k_z2 z - omega_2 t)")
    add_p("Dynamic surface normals are perturbed periodically, generating realistic undulating specular sunlight and moonlight glints across the water surface.")

    add_image_safe("screenshot_river.png", "Figure 7: River corridor showing dynamic water waves, mooring posts, and traditional sailboats.", width=Inches(5.6))

    add_h2("Agricultural Paddy Fields (ধানের ক্ষেত)")
    add_p("The southern terrain features terraced rice paddies divided by raised earthen bunds (আইল). Crop rows are modeled with alternating green chlorophyll hues, capturing authentic Bengali agrarian topography.")

    # ==================== CHAPTER 06 ====================
    add_h1("06  Hierarchical Articulated Modeling & Village Life")
    add_h2("Hierarchical Parent-Child Transformations")
    add_p("All articulated characters and composite mechanics follow strict hierarchical tree transformations where child coordinates inherit parent motion:")
    add_math("M_(child) = M_(parent) · M_(joint)(theta) · M_(local_offset)")

    add_h2("Traditional Majhi & Dingi Nouka (ডিঙি নৌকা)")
    add_p("The traditional river boat features a crescent wooden hull, bamboo Chhoi canopy, and a standing Majhi. The rowing cycle is mathematically linked to the elapsed animation time:")
    add_bullet("Oar Stroke Kinematics: The shoulder-elbow hierarchy drives the rowing paddle through an angular arc: theta_(oar)(t) = theta_(center) + A_(stroke) sin(omega_(row) t).")
    add_bullet("Buoyancy Response: The boat hull translates vertically and rolls: y_(bob)(t) = A_b sin(omega_b t), theta_(roll)(t) = A_r sin(omega_r t).")

    add_image_safe("object_images/04_dingi_boat.png", "Figure 8: Traditional Dingi Nouka with curved bamboo Chhoi hood.", width=Inches(4.0))
    add_image_safe("object_images/06_boatman_majhi.png", "Figure 9: Articulated procedural Majhi wearing bamboo Mathal hat.", width=Inches(3.2))

    add_h2("Courtyard Storytelling: Charpai, Hand Fan & Elders")
    add_p("In the central courtyard, village elders are seated on a hand-woven Charpai bed and cane Moras. An elder fans the family using an authentic palm-leaf hand fan (হাতের পাখা) whose blades rotate around a central handle pin. Children sit nearby reading Bengali books, recreating the nostalgic power-outage summer evening described in the project proposal.")

    add_image_safe("object_images/07_charpai_bed.png", "Figure 10: Braided rope Charpai cot with turned wooden legs.", width=Inches(4.0))
    add_image_safe("object_images/08_palm_fan_haat_pakha.png", "Figure 11: Traditional woven palm-leaf hand fan (Haat Pakha).", width=Inches(3.5))

    add_h2("Halchas (হালচাষ) & Thatched Cow Shed (Gowal Ghor)")
    add_p("Agricultural work is represented by a traditional plowing team: two humped deshi oxen (বলদ) harnessed with a wooden yoke (জোয়াল) and pulling a sharp wooden plow (লাঙ্গল), driven by a farmer. Adjacent lies the thatched cow shed with hay troughs and resting cows.")

    add_image_safe("screenshot_shifted_cowshed.png", "Figure 12: Thatched cow shed (Gowal Ghor) with feeding trough and farmyard cattle.", width=Inches(5.6))

    add_h2("River Fisherman Hunting Fish (জেলে)")
    add_p("A solitary fisherman stands at the riverbank operating a traditional cast net (খেপলা জাল), accompanied by a bamboo trap (পলো), woven creel (খালুই), and animated leaping silver fish (Key 'F').")

    add_image_safe("object_images/33_fisherman_hunting_fish.png", "Figure 13: River fisherman with cast net, bamboo gear, and jumping fish.", width=Inches(4.0))

    # ==================== CHAPTER 07 ====================
    add_h1("07  Vehicular Mechanics & Interactive Dynamics")
    add_h2("Bullock Cart (Gorur Gari / গরুর গাড়ি) Modeling")
    add_p("The bullock cart consists of a timber chassis, curved bamboo hood, rotating spoked wooden wheels, and a dual-oxen harness team. Users can inspect the cart (Key 'R'), trigger 180° wheel rotation steps (Key 'G'), or steer continuously using WASD keys.")
    add_p("Wheel rolling without slip satisfies the arc length constraint:")
    add_math("theta_(wheel)(t) = theta_0 + (1 / R_(wheel)) int v_(cart)(t) dt")
    add_p("where R_wheel = 0.65 m is the cart wheel radius.")

    add_image_safe("object_images/31_bullock_cart_gorur_gari.png", "Figure 14: Fully articulated traditional bullock cart (Gorur Gari).", width=Inches(4.5))

    add_h2("Interactive Tubewell (চাপকল / নলকূপ) Mechanics")
    add_p("The village tubewell features a cast-iron pump head, vertical cylinder, fulcrum handle, water spout, and concrete soak pit. Pressing Key 'P' triggers fulcrum handle pumping:")
    add_math("theta_(handle)(t) = theta_(rest) + A_(pump) sin(omega_(pump) t),   t in [0, T_(pump)]")
    add_p("Pumping activates animated water streaming from the spout into an earthen terracotta Kolshi (কলসি).")

    # ==================== CHAPTER 08 ====================
    add_h1("08  Interactive Camera Systems & Navigation")
    add_h2("Spherical Orbit Camera Mathematics")
    add_p("The primary camera orbits around a target look-at point T = [T_x, T_y, T_z]^T using spherical coordinates (yaw ψ, pitch θ, radius r):")
    add_math("P_(eye) = T + [ r cos theta sin psi,   r sin theta,   r cos theta cos psi ]^T")

    add_h2("View Matrix Derivation via Gram-Schmidt Orthonormalization")
    add_p("The camera coordinate frame is defined by forward f, right s, and true up u unit vectors:")
    add_math("f = (T - P_(eye)) / (||T - P_(eye)||),   s = (f xx u_(world)) / (||f xx u_(world)||),   u = s xx f")
    add_math("V = [ [s_x, s_y, s_z, -s · P_(eye)], [u_x, u_y, u_z, -u · P_(eye)], [-f_x, -f_y, -f_z, f · P_(eye)], [0, 0, 0, 1] ]")

    add_h2("Perspective Projection Matrix Formulation")
    add_p("The perspective frustum is parameterized by field of view (FOV = 45°), aspect ratio a = 16/9, near clip z_n = 0.1 m, and far clip z_f = 250.0 m:")
    add_math("P = [ [ 1 / (a tan(FOV/2)), 0, 0, 0 ], [ 0, 1 / tan(FOV/2), 0, 0 ], [ 0, 0, -(z_f + z_n)/(z_f - z_n), -(2 z_f z_n)/(z_f - z_n) ], [ 0, 0, -1, 0 ] ]")

    add_h2("Camera Navigation Controls")
    add_bullet("Mouse Left-Drag: Orbit around the active focal center (dynamic yaw and pitch).")
    add_bullet("Scroll Wheel: Smooth dolly zoom (adjusts camera distance r).")
    add_bullet("W / S / A / D / Arrow Keys: Continuous 3D fly-through translation across the terrain.")
    add_bullet("Keys '1' through '0': Instant bookmark switches to 10 prominent village landmarks.")
    add_bullet("SPACE: Automated cinematic fly-through tour with smooth cubic Hermite spline interpolation.")

    # ==================== CHAPTER 09 ====================
    add_h1("09  Illumination Models, Gouraud & Phong Shading")
    add_h2("The Blinn-Phong Illumination Equation")
    add_p("Total surface illumination I is evaluated as the sum of ambient, diffuse, specular, and emissive terms:")
    add_math("I = I_(ambient) + I_(diffuse) + I_(specular) + I_(emissive)")
    add_bullet("Ambient Term: I_ambient = k_a · L_a (uniform indirect environmental light).")
    add_bullet("Diffuse Term (Lambertian): I_diffuse = k_d · L_d · max(n · l, 0) (proportional to the cosine of the incidence angle).")
    add_bullet("Specular Term (Blinn-Phong): Evaluated using the half-vector h = (l + v) / ||l + v||:")
    add_math("I_(specular) = k_s · L_s · [ max(n · h, 0) ]^alpha")
    add_p("where α is the shininess exponent (α = 32.0 in our shaders).")

    add_h2("Point Light Inverse-Square Physical Attenuation")
    add_p("All 6 positional point lights (lanterns, cooking fire) incorporate realistic quadratic physical attenuation:")
    add_math("Att(d) = ( [ 1 - (d / d_(max))^2 ]^2 ) / ( k_c + k_l d + k_q d^2 )")
    add_p("where k_c = 1.0, k_l = 0.15–0.22, k_q = 0.025–0.040, and d_max = 15.0–24.0 m.")

    add_h2("Gouraud Shading vs. Phong Shading Comparison")
    add_p("A central requirement of the course curriculum is the practical implementation and comparison of Gouraud Shading and Phong Shading:")

    # Table of Shading Comparison
    s_table = doc.add_table(rows=1, cols=3)
    s_table.alignment = WD_TABLE_ALIGNMENT.CENTER
    s_hdr = s_table.rows[0].cells
    s_hdr[0].text = "Criterion"
    s_hdr[1].text = "Gouraud Shading (Per-Vertex)"
    s_hdr[2].text = "Phong Shading (Per-Fragment)"
    for cell in s_hdr:
        set_cell_background(cell, "0F386E")
        set_cell_margins(cell, 120, 120, 150, 150)
        p = cell.paragraphs[0]
        p.runs[0].font.color.rgb = RGBColor(0xFF, 0xFF, 0xFF)
        p.runs[0].font.bold = True
        p.runs[0].font.size = Pt(9.5)

    shading_cmp = [
        ("Calculation Stage", "Vertex Shader (at triangle vertices)", "Fragment Shader (at every rasterized pixel)"),
        ("Interpolation Property", "Hardware rasterizer linearly interpolates lighting intensities (colors) across triangle faces", "Hardware rasterizer interpolates normal vectors; lighting equation is re-evaluated per pixel"),
        ("Computational Cost", "Ultra-low: lighting calculated only N_vertices times; fragment shader executes negligible work", "Higher: dot products, normalization, and pow() evaluated N_fragments times"),
        ("Specular Highlights", "Can be softened or missed if vertex density is low and highlight falls within triangle interior", "Highly accurate and sharp; specular highlights rendered smoothly across curved silhouettes"),
        ("Silhouette & Quality", "Smooth color gradients; polygon edges can exhibit Mach banding under low mesh tessellation", "Photorealistic highlights; perfectly smooth curvature across surfaces"),
        ("Project Keybinding", "Active when Key 'M' toggles to Mode 1", "Active by default (Key 'M' toggles to Mode 0)"),
    ]

    for row_idx, data in enumerate(shading_cmp):
        row = s_table.add_row()
        bg = "F4F6F9" if row_idx % 2 == 1 else "FFFFFF"
        for c_idx, text in enumerate(data):
            cell = row.cells[c_idx]
            cell.text = text
            set_cell_background(cell, bg)
            set_cell_margins(cell, 80, 80, 100, 100)
            p = cell.paragraphs[0]
            p.runs[0].font.size = Pt(9)
            if c_idx == 0:
                p.runs[0].font.bold = True

    add_h2("Light Inside Object vs. Object Inside Light")
    add_p("A dedicated topic in the laboratory syllabus addresses the distinction between emissive objects and illuminated objects:")
    add_bullet("Light Inside Object (Emissive): Lantern flames, glowing charcoal embers in the clay stove, the Full Moon, and twinkling fireflies have emissive factor e = 1.0. These bypass external shadow/diffuse math and radiate at pure object color.")
    add_bullet("Object Inside Light (Receptor): Houses, boats, trees, and ground have e = 0.0 and depend strictly on ambient, diffuse, and specular terms.")

    # ==================== CHAPTER 10 ====================
    add_h1("10  Whitted-Style Recursive Ray Tracing Engine (Bonus Feature)")
    add_h2("Ray Representation & Primary Ray Generation")
    add_p("Implemented as an advanced bonus feature, pressing Key 'Y' switches the renderer to a real-time Whitted-style Ray Tracing engine executing entirely within GLSL 330 Core. Primary camera rays are cast through screen pixel coordinates:")
    add_math("R(t) = O + t · D,   t > 0")
    add_p("where O is the camera world position and D is the normalized direction vector through pixel (x, y).")

    add_h2("Analytic Ray-Geometry Intersections")
    add_bullet("Ray-Sphere Intersection: For a sphere centered at C with radius r, ||O + t D - C||^2 = r^2 yields a quadratic equation a t^2 + b t + c = 0. The discriminant Δ = b^2 - 4ac determines intersection points.")
    bullet_box = add_bullet("Ray-Box (AABB) Slab Method: For axis-aligned boxes [B_min, B_max], ray entry and exit intervals along X, Y, Z slabs are computed: t_near = max( min(t_x1, t_x2), min(t_y1, t_y2), min(t_z1, t_z2) ).")

    add_h2("Direct Occlusion Shadow Rays & Specular Mirror Reflections")
    add_p("At every intersection point P, a secondary shadow ray is cast toward light position L_pos. If blocked by any intervening geometry, the direct diffuse and specular illumination is clamped to zero, generating perfectly sharp geometric shadows.")
    add_p("For reflective surfaces (such as the river water and metallic lanterns), secondary reflection rays are spawned recursively:")
    add_math("D_(reflect) = D - 2 (D · N) N")
    add_math("I_(total) = I_(direct) + k_(spec) · I_(reflect)")

    # ==================== CHAPTER 11 ====================
    add_h1("11  Texture Mapping & Procedural Surface Synthesizers")
    add_h2("UV Mapping & Texture Modes")
    add_p("Pressing Key 'X' dynamically cycles between three surface representation modes:")
    add_bullet("Mode 0: Solid Shading (Pure geometric diffuse colors for structural validation).")
    add_bullet("Mode 1: Procedural GLSL Detailing (Real-time GPU mathematical grain without memory overhead).")
    add_bullet("Mode 2: GPU 2D Texture Maps (Sampled UV texture buffers).")

    add_h2("Procedural Mathematical Synthesizers")
    add_bullet("Wood Grain: Synthesized using high-frequency sine perturbation: color = base · (0.82 + 0.32 · sin(36 y + 2.2 · sin(14 x + 14 z))).")
    add_bullet("Brick & Mortar Joints: Modulo fractional cell stepped functions with moisture/algae height damping.")
    add_bullet("Bamboo Weave: Interleaved biaxial sinusoidal fiber splitting.")

    # ==================== CHAPTER 12 ====================
    add_h1("12  Performance Optimization & Codebase Architecture")
    add_h2("High-Performance Engineering Implementations")
    add_bullet("Zero Heap Allocation Uniform Lookups: Modern C++20 transparent string hashing (TransparentStringHash, TransparentStringEqual) eliminates std::string dynamic heap allocations during runtime uniform setting.")
    bullet_pre = add_bullet("Precomputed Village Transforms: Model transformation matrices for static houses are precomputed once on startup rather than recomputed across 60 FPS render passes.")
    bullet_vao = add_bullet("Driver State Caching: VAO binding states are cached to eliminate redundant OpenGL driver context switches.")
    bullet_hw = add_bullet("Hardware Iconification Throttling: When minimized, the application sleeps via glfwWaitEventsTimeout, reducing idle CPU/GPU usage to 0%.")

    add_h2("Source Code Organization")
    add_bullet("main.cpp: Application lifecycle, input dispatching, scene composition, and primary render loop.")
    add_bullet("src/Shader.cpp / include/Shader.h: GLSL compilation, uniform management, Gouraud and Phong vertex/fragment sources.")
    add_bullet("src/Primitives.cpp: Canonical geometric meshes (cube, cylinder, cone, sphere, hemisphere, pyramid, arch).")
    add_bullet("src/objects/CurvedObject.cpp: Cubic Bézier parametric curve evaluation and surface of revolution mesh generation.")
    add_bullet("src/objects/RayTracer.cpp: Full-screen Whitted ray tracing engine in GLSL.")
    add_bullet("src/objects/Boat.cpp, House.cpp, Mosque.cpp, Tree.cpp, etc.: Modular procedural village asset components.")

    # ==================== CHAPTER 13 ====================
    add_h1("13  Limitations, Future Roadmap & Conclusion")
    add_h2("Known Limitations")
    add_bullet("Rasterized Shadow Mapping: While the Ray Tracing mode (Key 'Y') computes exact ray-traced shadows, the default rasterized Blinn-Phong pass relies on ambient attenuation rather than depth-buffer shadow maps.")
    add_bullet("Interior House Modeling: Homesteads are constructed with closed exterior wall and roof planes without detailed interior domestic furniture.")

    add_h2("Future Enhancements")
    add_bullet("Cascaded Shadow Maps (CSM) integrated directly into the rasterization pipeline.")
    add_bullet("Particle compute shaders for realistic monsoon rain simulation with river ripple physics.")
    add_bullet("Audio synthesis incorporating authentic rural ambient soundscapes (river flow, bird calls, evening crickets).")

    add_h2("Conclusion")
    add_p("The 'Village Gathering by the River' project successfully demonstrates the comprehensive breadth of 3D computer graphics principles taught in the CSE 4102 curriculum. By strictly adhering to procedural mathematical generation—constructing every building, character, vehicle, animal, tree, and wave without external 3D model loaders—the project exemplifies core transformation pipelines, hierarchical articulated motion, dual Gouraud and Phong shading models, multi-light attenuation, parametric Bézier surfaces, and advanced real-time ray tracing. The application stands as an authentic, interactive, and academically rigorous showcase of modern OpenGL engineering.")

    # ==================== CHAPTER 14 ====================
    add_h1("14  References")
    references = [
        "[1] H. Gouraud, \"Continuous shading of curved surfaces,\" IEEE Transactions on Computers, vol. C-20, no. 6, pp. 623–629, June 1971.",
        "[2] B. T. Phong, \"Illumination for computer generated pictures,\" Communications of the ACM, vol. 18, no. 6, pp. 311–317, June 1975.",
        "[3] J. F. Blinn, \"Models of light reflection for computer synthesized pictures,\" in Proceedings of the 4th Annual Conference on Computer Graphics and Interactive Techniques (SIGGRAPH '77), pp. 192–198, 1977.",
        "[4] T. Whitted, \"An improved illumination model for shaded display,\" Communications of the ACM, vol. 23, no. 6, pp. 343–349, June 1980.",
        "[5] T. L. Kay and J. T. Kajiya, \"Ray tracing complex scenes,\" in Proceedings of the 13th Annual Conference on Computer Graphics and Interactive Techniques (SIGGRAPH '86), pp. 269–278, 1986.",
        "[6] P. Bézier, \"Numerical Code for Numerical Control: Definition of Surfaces,\" Proceedings of the Institution of Mechanical Engineers, 1971.",
        "[7] G. Farin, Curves and Surfaces for Computer-Aided Geometric Design: A Practical Guide, 5th ed., San Francisco, CA: Morgan Kaufmann, 2002.",
        "[8] A. Fournier and W. T. Reeves, \"A simple model of ocean waves,\" in Proceedings of SIGGRAPH '86, vol. 20, no. 4, pp. 75–84, 1986.",
        "[9] M. Finch, \"Effective water simulation from physical models,\" in GPU Gems: Programming Techniques, Tips and Tricks for Real-Time Graphics, ch. 1, Addison-Wesley, 2004.",
        "[10] J. D. Foley, A. van Dam, S. K. Feiner, and J. F. Hughes, Computer Graphics: Principles and Practice, 3rd ed., Boston: Addison-Wesley, 2013.",
        "[11] D. Shreiner, G. Sellers, J. Kessenich, and B. Licea-Kane, OpenGL Programming Guide: The Official Guide to Learning OpenGL, Version 4.3, 8th ed., Addison-Wesley, 2013.",
        "[12] J. Kessenich, D. Baldwin, and R. Rost, The OpenGL Shading Language, Language Version 3.30, The Khronos Group, 2010. [Online]. Available: https://registry.khronos.org/OpenGL/specs/gl/GLSLangSpec.3.30.pdf",
        "[13] J. de Vries, LearnOpenGL: Learn Modern OpenGL Graphics Programming in a Step-by-Step Fashion, 2020. [Online]. Available: https://learnopengl.com/",
        "[14] Khulna University of Engineering & Technology (KUET), CSE 4102: Computer Graphics and Image Processing Laboratory Course Curriculum & Lab Guidelines, Department of Computer Science and Engineering, 2026."
    ]
    for ref in references:
        add_p(ref)

    # Save document
    out_path = "Village_Gathering_By_The_River_Project_Report.docx"
    doc.save(out_path)
    print(f"Report successfully generated and saved to: {out_path}")

create_report()
