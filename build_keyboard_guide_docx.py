# -*- coding: utf-8 -*-
"""
build_keyboard_guide_docx.py
Generates a comprehensive, beautifully formatted Word (.docx) document
containing all keyboard controls, shift parameters, and functionality in Bangla.
"""

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

def set_cell_margins(cell, top=120, bottom=120, left=160, right=160):
    tcPr = cell._tc.get_or_add_tcPr()
    tcMar = parse_xml(f'<w:tcMar {nsdecls("w")}><w:top w:w="{top}" w:type="dxa"/><w:bottom w:w="{bottom}" w:type="dxa"/><w:left w:w="{left}" w:type="dxa"/><w:right w:w="{right}" w:type="dxa"/></w:tcMar>')
    tcPr.append(tcMar)

def set_table_borders(table, color="CCCCCC", sz="4", val="single"):
    tblPr = table._tbl.tblPr
    borders = parse_xml(
        f'<w:tblBorders {nsdecls("w")}>'
        f'<w:top w:val="{val}" w:sz="{sz}" w:space="0" w:color="{color}"/>'
        f'<w:bottom w:val="{val}" w:sz="{sz}" w:space="0" w:color="{color}"/>'
        f'<w:insideH w:val="{val}" w:sz="{sz}" w:space="0" w:color="{color}"/>'
        f'<w:insideV w:val="none"/>'
        f'<w:left w:val="none"/>'
        f'<w:right w:val="none"/>'
        f'</w:tblBorders>'
    )
    tblPr.append(borders)

def build_docx(output_path):
    doc = docx.Document()

    # Page setup - A4 with standard 0.75 in margins
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
    font.size = Pt(10.5)
    font.color.rgb = RGBColor(0x22, 0x22, 0x22)

    # Header / Title Box
    title_p = doc.add_paragraph()
    title_p.alignment = WD_ALIGN_PARAGRAPH.CENTER
    title_p.paragraph_format.space_before = Pt(0)
    title_p.paragraph_format.space_after = Pt(2)
    run_inst = title_p.add_run("খুলনা প্রকৌশল ও প্রযুক্তি বিশ্ববিদ্যালয় (KUET)\nকম্পিউটার সায়েন্স অ্যান্ড ইঞ্জিনিয়ারিং বিভাগ")
    run_inst.font.name = 'Arial'
    run_inst.font.size = Pt(13)
    run_inst.bold = True
    run_inst.font.color.rgb = RGBColor(0x1B, 0x36, 0x5D)

    sub_p = doc.add_paragraph()
    sub_p.alignment = WD_ALIGN_PARAGRAPH.CENTER
    sub_p.paragraph_format.space_before = Pt(2)
    sub_p.paragraph_format.space_after = Pt(4)
    run_crs = sub_p.add_run("কোর্স: CSE 4100 (Computer Graphics & Image Processing Laboratory)")
    run_crs.font.name = 'Calibri'
    run_crs.font.size = Pt(11)
    run_crs.bold = True
    run_crs.font.color.rgb = RGBColor(0x44, 0x44, 0x44)

    proj_p = doc.add_paragraph()
    proj_p.alignment = WD_ALIGN_PARAGRAPH.CENTER
    proj_p.paragraph_format.space_before = Pt(2)
    proj_p.paragraph_format.space_after = Pt(14)
    run_proj = proj_p.add_run("প্রজেক্ট: Village Gathering by the River (Bangladeshi Rural Night Scene in 3D)\nরোল নং: 2107064 | গ্রাফিক্স পাইপলাইন: OpenGL 3.3 Core Profile (GLSL 330)")
    run_proj.font.name = 'Calibri'
    run_proj.font.size = Pt(10.5)
    run_proj.font.color.rgb = RGBColor(0x2B, 0x54, 0x7E)

    # Document Title Ribbon
    banner_p = doc.add_paragraph()
    banner_p.alignment = WD_ALIGN_PARAGRAPH.CENTER
    banner_p.paragraph_format.space_before = Pt(6)
    banner_p.paragraph_format.space_after = Pt(14)
    run_b = banner_p.add_run("সম্পূর্ণ কীবোর্ড কন্ট্রোলস, প্যারামিটার ও ইন্টারঅ্যাকশন ম্যানুয়াল")
    run_b.font.name = 'Arial'
    run_b.font.size = Pt(16)
    run_b.bold = True
    run_b.font.color.rgb = RGBColor(0x0E, 0x4D, 0x92)

    def add_section_header(title, color_hex="0E4D92"):
        p = doc.add_paragraph()
        p.paragraph_format.space_before = Pt(14)
        p.paragraph_format.space_after = Pt(6)
        p.paragraph_format.keep_with_next = True
        run = p.add_run(title)
        run.font.name = 'Arial'
        run.font.size = Pt(13)
        run.bold = True
        run.font.color.rgb = RGBColor(int(color_hex[0:2], 16), int(color_hex[2:4], 16), int(color_hex[4:6], 16))
        return p

    def add_callout(text, bg_hex="F0F4F8", border_hex="2B547E"):
        tbl = doc.add_table(rows=1, cols=1)
        tbl.alignment = WD_TABLE_ALIGNMENT.CENTER
        tbl.autofit = False
        tbl.columns[0].width = Inches(6.77)
        cell = tbl.cell(0, 0)
        set_cell_background(cell, bg_hex)
        set_cell_margins(cell, top=100, bottom=100, left=150, right=150)
        
        tcPr = cell._tc.get_or_add_tcPr()
        borders = parse_xml(
            f'<w:tcBorders {nsdecls("w")}>'
            f'<w:left w:val="single" w:sz="24" w:space="0" w:color="{border_hex}"/>'
            f'<w:top w:val="none"/>'
            f'<w:bottom w:val="none"/>'
            f'<w:right w:val="none"/>'
            f'</w:tcBorders>'
        )
        tcPr.append(borders)
        
        p = cell.paragraphs[0]
        p.paragraph_format.space_before = Pt(0)
        p.paragraph_format.space_after = Pt(0)
        run = p.add_run(text)
        run.font.name = 'Calibri'
        run.font.size = Pt(10)
        run.font.color.rgb = RGBColor(0x1A, 0x25, 0x30)
        
        p_after = doc.add_paragraph()
        p_after.paragraph_format.space_before = Pt(0)
        p_after.paragraph_format.space_after = Pt(4)

    # ─────────────────────────────────────────────────────────────
    # SECTION 1: স্যারের রিকোয়ার্ড কোর টেকনিক্যাল ফিচার কি
    # ─────────────────────────────────────────────────────────────
    add_section_header("১. স্যারের রিকোয়ার্ড কোর টেকনিক্যাল ফিচার কি (Core Lab Requirements)")
    add_callout(
        "বিশেষ দ্রষ্টব্য: এই সেকশনের কি-গুলো স্যারের মূল রিকোয়ারমেন্টের সাথে সরাসরি সম্পর্কিত (Gouraud vs Phong Shading, Ray Tracing, Lighting Models, Wireframe ও Texture Modes)। ভাইভাতে এই কি-গুলো সবার আগে দেখাতে হবে।"
    )

    t1 = doc.add_table(rows=1, cols=4)
    t1.alignment = WD_TABLE_ALIGNMENT.CENTER
    t1.autofit = False
    col_w1 = [Inches(0.9), Inches(2.2), Inches(2.4), Inches(1.27)]
    for j, w in enumerate(col_w1):
        t1.columns[j].width = w

    headers1 = ["কীবোর্ড কি", "ফিচার ও কাজের বিবরণ (Bangla)", "প্রযুক্তিগত বাস্তবায়ন ও থিওরি", "কোড রেফারেন্স"]
    hdr_cells1 = t1.rows[0].cells
    for j, h in enumerate(headers1):
        hdr_cells1[j].width = col_w1[j]
        set_cell_background(hdr_cells1[j], "1B365D")
        set_cell_margins(hdr_cells1[j], top=100, bottom=100, left=120, right=120)
        p = hdr_cells1[j].paragraphs[0]
        p.alignment = WD_ALIGN_PARAGRAPH.CENTER
        run = p.add_run(h)
        run.bold = True
        run.font.name = 'Arial'
        run.font.size = Pt(10)
        run.font.color.rgb = RGBColor(0xFF, 0xFF, 0xFF)

    data1 = [
        (
            "Key 'M'",
            "Gouraud Shading বনাম Phong Shading টগল।\n• Phong (০): পার-ফ্র্যাগমেন্ট লাইটিং (ডিফল্ট)\n• Gouraud (১): পার-ভার্টেক্স লাইটিং",
            "Phong মোডে প্রতি পিক্সেলে নরমাল ভেক্টর হিসেব করে Blinn-Phong স্পেকুলার হাইলাইট নিখুঁত রাখা হয়। Gouraud মোডে ভার্টেক্সে আলো হিসেব করে রাস্টারাইজারে লিনিয়ার ইন্টারপোলেশন হয়।",
            "main.cpp: 604-623\nShader.cpp: 110-156"
        ),
        (
            "Key 'Y'",
            "রিয়েল-টাইম Whitted Ray Tracing ইঞ্জিন অন/অফ (স্যারের বোনাস ও এক্সট্রা মার্কস ফিচার)।",
            "রাস্টারাইজেশন বাইপাস করে ফুল-স্ক্রিন পিক্সেল গ্রিড দিয়ে Primary Rays নিক্ষেপ, Ray-Sphere/Box Intersection, Shadow Rays ও পানিতে Specular Reflection Rays হিসাব করে।",
            "main.cpp: 585-602\nRayTracer.cpp: 35-240"
        ),
        (
            "Key 'L'",
            "৪টি লাইটিং ও অ্যাটমোস্ফিয়ার মোড সাইকেল:\n• ০: চাঁদের রাত (Moonlit Night - ডিফল্ট)\n• ১: রোদঝলমল দিন (Radiant Day)\n• ২: আনলিট ৩ডি ফ্যাসেট (Unlit 3D Inspection)\n• ৩: ফ্ল্যাট অবজেক্ট কালার (Pure Flat Color)",
            "Lighting Mode পরিবর্তন করে ডিরেকশনাল লাইট কালার, ইনটেনসিটি, অ্যাম্বিয়েন্ট ব্যালেন্স, ফগ ও স্কাইবক্স পরিবর্তন করা হয়। আনলিট মোডে জ্যামিতিক অবজেক্ট পরীক্ষা করা যায়।",
            "main.cpp: 390-399\nmain.cpp: 1815-1890"
        ),
        (
            "Key 'U'",
            "Hard Light বনাম Soft Light টগল।\n• Hard: কড়া ছায়া, শার্প কাটিং, তীব্র হাইলাইট\n• Soft: মৃদু আলো, নরম ডিফিউজ ছায়া ও প্রশস্ত অ্যাম্বিয়েন্ট",
            "Hard Light মোডে ল্যাম্বার্ট কোসাইনের এক্সপোনেন্ট (pow 1.35) ও শার্প টার্মিনেটর ব্যবহার করা হয়। Soft Light-এ র‍্যাপ-অ্যারাউন্ড ডিফিউজ এবং হাই অ্যাম্বিয়েন্ট ব্যবহৃত হয়।",
            "main.cpp: 405-412\nShader.cpp: 324-353"
        ),
        (
            "Key 'Z'",
            "Wireframe মোড অন/অফ।\n• glPolygonMode(GL_FRONT_AND_BACK, GL_LINE)\n• Solid Surface: GL_FILL",
            "থ্রিডি পলিমডেলের প্রতিটি ট্রায়াঙ্গেল মেশ ও ভার্টেক্স কানেকশন খালি চোখে দেখার জন্য ল্যাব মাইলস্টোন ইন্সপেকশনে এটি ব্যবহৃত হয়।",
            "main.cpp: 414-419"
        ),
        (
            "Key 'X'",
            "৩টি Texture মোড সাইকেল:\n• ০: Solid Color (ক্লিন জ্যামিতি)\n• ১: Procedural GLSL (রিয়েল-টাইম টেক্সচার)\n• ২: GPU Texture Maps (ডিফল্ট ইমেজ ম্যাপ)",
            "প্রসিডিউরাল মোডে সাইন/নয়েজ ফাংশন দিয়ে কাঠের আঁশ, বাঁশের বুনন ও মাটির টেক্সচার তৈরি হয়। মোড ২-এ বিটম্যাপ টেক্সচার স্যাম্পল করা হয়।",
            "main.cpp: 544-552\nShader.cpp: 237-294"
        ),
        (
            "Key 'T'",
            "টেরাইন ও নদী ভিজিবিলিটি অন/অফ (Terrain Ground & River Water Toggle)।",
            "মাটি ও নদী হাইড করে শুধুমাত্র ফ্রি-স্ট্যান্ডিং অবজেক্ট ও তাদের নিজস্ব স্ট্রাকচার আলাদা করে পর্যবেক্ষণ করতে সাহায্য করে।",
            "main.cpp: 401-404"
        ),
        (
            "Key 'C'",
            "প্রজেক্টের সকল ৩ডি অবজেক্টের একক স্ক্রিনশট স্বয়ংক্রিয়ভাবে এক্সপোর্ট (BMP File Exporter)।",
            "প্রতিটি মডেলকে এককভাবে ফ্রেম করে `glReadPixels()` দিয়ে হাই-রেজোলিউশন বিটম্যাপ ইমেজ ফাইলে সেভ করে।",
            "main.cpp: 580-584\nmain.cpp: 791-1175"
        )
    ]

    for row_idx, rdata in enumerate(data1):
        row = t1.add_row()
        bg_col = "F9FBFD" if (row_idx % 2 == 1) else "FFFFFF"
        for c_idx, text in enumerate(rdata):
            cell = row.cells[c_idx]
            cell.width = col_w1[c_idx]
            set_cell_background(cell, bg_col)
            set_cell_margins(cell, top=80, bottom=80, left=120, right=120)
            p = cell.paragraphs[0]
            p.paragraph_format.space_before = Pt(0)
            p.paragraph_format.space_after = Pt(0)
            run = p.add_run(text)
            run.font.name = 'Calibri'
            run.font.size = Pt(9.5)
            if c_idx == 0:
                run.bold = True
                run.font.color.rgb = RGBColor(0x0E, 0x4D, 0x92)

    set_table_borders(t1)

    # ─────────────────────────────────────────────────────────────
    # SECTION 2: যানবাহন ও ইন্টারেক্টিভ অবজেক্ট কন্ট্রোল
    # ─────────────────────────────────────────────────────────────
    add_section_header("২. যানবাহন ও ইন্টারেক্টিভ অবজেক্ট কন্ট্রোল (Vehicle Shifts & Interaction Mechanics)")
    add_callout(
        "স্যারের নির্দেশনা: 'Interaction বলতে user input দিয়ে কিছু control করা'। নিচে N, G, H এবং WASD কি-এর মাধ্যমে প্রতি ক্লিকে ঠিক কতটুকু দূরত্ব শিফট হয় এবং কীভাবে কন্ট্রোল কাজ করে তা উল্লেখ করা হলো।"
    )

    t2 = doc.add_table(rows=1, cols=4)
    t2.alignment = WD_TABLE_ALIGNMENT.CENTER
    t2.autofit = False
    col_w2 = [Inches(1.0), Inches(2.1), Inches(2.4), Inches(1.27)]
    for j, w in enumerate(col_w2):
        t2.columns[j].width = w

    headers2 = ["কীবোর্ড কি", "ফিচার ও শিফট দূরত্ব (Shift Amount)", "কাজের অভ্যন্তরীণ মেকানিজম ও ফিজিক্স", "কোড ভ্যারিয়েবল ও লাইন"]
    hdr_cells2 = t2.rows[0].cells
    for j, h in enumerate(headers2):
        hdr_cells2[j].width = col_w2[j]
        set_cell_background(hdr_cells2[j], "1B4D3E")  # Dark Forest Green Header
        set_cell_margins(hdr_cells2[j], top=100, bottom=100, left=120, right=120)
        p = hdr_cells2[j].paragraphs[0]
        p.alignment = WD_ALIGN_PARAGRAPH.CENTER
        run = p.add_run(h)
        run.bold = True
        run.font.name = 'Arial'
        run.font.size = Pt(10)
        run.font.color.rgb = RGBColor(0xFF, 0xFF, 0xFF)

    data2 = [
        (
            "Key 'G'",
            "গরুর গাড়ি স্টেপ এডভান্স (Bullock Cart Step Advance):\n• চাকার ঘূর্ণন: ডায়নামিক হাফ-টার্ন\n• শিফট দূরত্ব: ৩.৮০ মিটার (3.80 m)",
            "Rolling without slipping নিয়ম মেনে: Δs = R · Δθ। প্রতি ক্লিকে চাকা দ্রুত ঘুরে গাড়ি ৩.৮০ মিটার সামনে অত্যন্ত চটপটে গতিতে এগিয়ে যায় এবং বলদ গরুর পায়ে হাঁটার স্ট্রাইড এনিমেশন চলে। ক্যামেরা গাড়িকে চেজ ভিউতে অনুসরণ করে।",
            "main.cpp: 287-291\nmain.cpp: 645-671\nmain.cpp: 1492-1513\n(CART_STEP_DIST)"
        ),
        (
            "Key 'N'",
            "ডিঙি নৌকা স্টেপ এডভান্স (Country Boat Step Advance):\n• শিফট দূরত্ব: ৪.০ মিটার (4.0 m)\n• বৈঠা বাওয়া: ১টি সম্পূর্ণ ৩৬০° (2π rad) স্ট্রোক সাইকেল",
            "প্রতি ক্লিকে নৌকা নদীর স্রোতের দিকে ৪.০ মিটার দ্রুত ও শক্তিশালী টানে সামনে এগিয়ে যায়। একই সাথে মাঝির দুই হাত ও বৈঠা দিয়ে পানিতে একটি ছন্দময় বৈঠা বাওয়ার স্ট্রোক পূর্ণ হয়। অন্যান্য নৌকা ও তীরের সাথে সংঘর্ষ এড়ানোর জন্য কোলিশন সিস্টেম সক্রিয় থাকে।",
            "main.cpp: 305-309\nmain.cpp: 673-696\nmain.cpp: 1579-1601\n(BOAT_STEP_DIST)"
        ),
        (
            "Key 'H'",
            "হালচাষ মাঠ স্টেপ এডভান্স (Halchas Ox Plowing Advance):\n• শিফট দূরত্ব: ২.৮০ মিটার (2.80 m)\n• জোড়া বলদ ও কৃষকের পায়ের স্টেপ",
            "কৃষিক্ষেত্রে ঐতিহ্যবাহী হালচাষ। প্রতি ক্লিকে জোড়া বলদ গরু, কাঠের লাঙল ও কৃষক ২.৮০ মিটার সামনে মাটি চষে চটপটে পদক্ষেপে এগোয় এবং পায়ের হাঁটার ফেজ লিনিয়ারলি আপডেট হয়। ফসলের সীমানার বাইরে যাওয়া প্রতিরোধে বাউন্ডিং বক্স সক্রিয়।",
            "main.cpp: 322-326\nmain.cpp: 698-722\nmain.cpp: 1743-1753\n(PLOW_STEP_DIST)"
        ),
        (
            "Key 'R'",
            "গরুর গাড়ির ড্রাইভিং মোডে প্রবেশ ও চেজ ক্যামেরা অ্যাক্টিভেশন।",
            "ক্যামেরা স্বয়ংক্রিয়ভাবে গ্রামীণ রাস্তায় গরুর গাড়ির পেছনে ৯.২ মিটার দূরত্বে চলে যায় এবং গাড়িকে সার্বক্ষণিক কেন্দ্রবিন্দুতে রেখে স্মুথ ট্র্যাকিং করে।",
            "main.cpp: 624-644"
        ),
        (
            "Key 'P'",
            "চাপকল (টিউবওয়েল) পাম্পিং অ্যাকশন:\n• পাম্প সক্রিয় সময়: ৪.৫ সেকেন্ড (4.5s)\n• হাতল ওঠানামা ও মাটির কলসিতে পানি পড়ার এনিমেশন",
            "চাপকলের লোহার হাতল যান্ত্রিকভাবে উপরে-নিচে দুলতে থাকে, মুখ দিয়ে ভূগর্ভস্থ স্বচ্ছ পানি প্রবাহিত হয় এবং নিচে রাখা মাটির কলসিতে পানি পড়ার প্রভাব সৃষ্টি হয়।",
            "main.cpp: 348-360\n(g_pumpTimer = 4.5f)"
        ),
        (
            "W / Up Arrow",
            "গাড়ি / নৌকা / হালচাষে লাগাতার সামনে চলা (Continuous Forward Drive / Throttle)।",
            "গাড়ির ত্বরণ CART_ACCEL = 32.0 m/s², সর্বোচ্চ গতি ১৮.০ m/s। নৌকার ত্বরণ BOAT_ACCEL = 26.0 m/s², সর্বোচ্চ গতি ২০.০ m/s।",
            "main.cpp: 1451-1470\nmain.cpp: 1543-1557"
        ),
        (
            "S / Down Arrow",
            "গাড়ি / নৌকা / হালচাষে ব্রেক ও রিভার্স (Brake / Reverse Propulsion)।",
            "চাপলে গাড়ির গতি দ্রুত কমে এবং রিভার্সে যেতে পারে। নৌকার ব্যাকওয়ার্ড ড্র্যাগ ও রিভার্স সক্রিয় হয়।",
            "main.cpp: 1453-1473\nmain.cpp: 1545-1559"
        ),
        (
            "A / Left Arrow",
            "গাড়ির স্টিয়ারিং বামে ঘোরানো / নৌকার হাল বামে ঘোরানো (Steer Left)।",
            "গাড়ির স্টিয়ারিং টার্নিং স্পিড ১২০°/সেকেন্ড। নৌকার রাডার স্টিয়ারিং স্পিড ৩.৬ রেডিয়ান/সেকেন্ড।",
            "main.cpp: 1455-1489\nmain.cpp: 1547-1576"
        ),
        (
            "D / Right Arrow",
            "গাড়ির স্টিয়ারিং ডানে ঘোরানো / নৌকার হাল ডানে ঘোরানো (Steer Right)।",
            "গাড়ির স্টিয়ারিং টার্নিং স্পিড ১২০°/সেকেন্ড। নৌকার রাডার স্টিয়ারিং স্পিড ৩.৬ রেডিয়ান/সেকেন্ড।",
            "main.cpp: 1457-1489\nmain.cpp: 1549-1576"
        )
    ]

    for row_idx, rdata in enumerate(data2):
        row = t2.add_row()
        bg_col = "F5FAF7" if (row_idx % 2 == 1) else "FFFFFF"
        for c_idx, text in enumerate(rdata):
            cell = row.cells[c_idx]
            cell.width = col_w2[c_idx]
            set_cell_background(cell, bg_col)
            set_cell_margins(cell, top=80, bottom=80, left=120, right=120)
            p = cell.paragraphs[0]
            p.paragraph_format.space_before = Pt(0)
            p.paragraph_format.space_after = Pt(0)
            run = p.add_run(text)
            run.font.name = 'Calibri'
            run.font.size = Pt(9.5)
            if c_idx == 0:
                run.bold = True
                run.font.color.rgb = RGBColor(0x1B, 0x4D, 0x3E)

    set_table_borders(t2)

    # ─────────────────────────────────────────────────────────────
    # SECTION 3: এনভায়রনমেন্ট ও লাইটিং সোর্স কন্ট্রোল
    # ─────────────────────────────────────────────────────────────
    add_section_header("৩. এনভায়রনমেন্ট ও লাইটিং সোর্স কন্ট্রোল (Lighting & Environment Keys)")

    t3 = doc.add_table(rows=1, cols=4)
    t3.alignment = WD_TABLE_ALIGNMENT.CENTER
    t3.autofit = False
    col_w3 = [Inches(1.0), Inches(2.1), Inches(2.4), Inches(1.27)]
    for j, w in enumerate(col_w3):
        t3.columns[j].width = w

    headers3 = ["কীবোর্ড কি", "ফিচার ও কাজের বিবরণ (Bangla)", "টেকনিক্যাল প্রভাব ও মান", "কোড ভ্যারিয়েবল"]
    hdr_cells3 = t3.rows[0].cells
    for j, h in enumerate(headers3):
        hdr_cells3[j].width = col_w3[j]
        set_cell_background(hdr_cells3[j], "7A3803")  # Deep Amber/Brown Header
        set_cell_margins(hdr_cells3[j], top=100, bottom=100, left=120, right=120)
        p = hdr_cells3[j].paragraphs[0]
        p.alignment = WD_ALIGN_PARAGRAPH.CENTER
        run = p.add_run(h)
        run.bold = True
        run.font.name = 'Arial'
        run.font.size = Pt(10)
        run.font.color.rgb = RGBColor(0xFF, 0xFF, 0xFF)

    data3 = [
        (
            "Key 'J'",
            "Directional Light (চাঁদ বা সূর্যের আলো) অন/অফ টগল।",
            "ডিরেকশনাল লাইট বন্ধ করলে দৃশ্যপটে শুধু পয়েন্ট লাইটের (লণ্ঠন ও চুলার আগুন) আলো দৃশ্যমান থাকে, যা দিয়ে খাঁটি পয়েন্ট লাইট অ্যাটেনুয়েশন পরীক্ষা করা যায়।",
            "main.cpp: 353-358\n(g_dirLightEnabled)"
        ),
        (
            "Key 'O'",
            "গ্রামের ৬টি Point Light ৪টি ধাপে টগল:\n• ০: Normal Golden Glow (1.0x)\n• ১: Extra Bright Blazing Flame (1.85x)\n• ২: Soft Low Amber Glow (0.45x)\n• ৩: Extinguished (Point Lights OFF)",
            "৬টি পয়েন্ট লাইট অবস্থান: ১. কেন্দ্রীয় উঠান, ২. বাঁধা নৌকা, ৩. চলমান ডিঙি নৌকা, ৪. মসজিদ চত্বর, ৫. রান্নাঘরের মাটির চুলা, ৬. নদীর ঘাট। প্রতিটিতে ফিজিক্যাল ইনভার্স-স্কয়ার ক্ষয় রয়েছে।",
            "main.cpp: 360-369\n(g_lanternMode)"
        ),
        (
            "Key 'B'",
            "গ্রীষ্মের মৃদু বাতাস (Summer Breeze Wind Sway) অন/অফ।",
            "গাছের গুঁড়ি ও ডালপালায় সময়ভিত্তিক সাইন ওয়েভ দোলন বন্ধ বা চালু করে। বাতাস চালু থাকলে নারিকেল, সুপারি ও বাঁশঝাড় প্রাকৃতিকভাবে দোলে।",
            "main.cpp: 370-374\n(g_windBreeze)"
        ),
        (
            "Key '['",
            "এনিমেশন গতি হ্রাস (Slow-motion):\n• প্রতি ক্লিকে -০.২৫x করে গতি কমে (সর্বনিম্ন ০.২০x)।",
            "পানির স্রোত, মাঝির বৈঠা, ডালপালার দোলা ও প্রাণীদের চলাফেরা স্লো-মোশনে নিখুঁতভাবে পর্যবেক্ষণ করতে ব্যবহৃত হয়।",
            "main.cpp: 376-379\n(g_animSpeed)"
        ),
        (
            "Key ']'",
            "এনিমেশন গতি বৃদ্ধি (Fast-forward):\n• প্রতি ক্লিকে +০.২৫x করে গতি বাড়ে (সর্বোচ্চ ৩.০০x)।",
            "নদীর ঢেউয়ের বেগ ও নৌকার ছন্দময় চলাফেরা দ্রুত লুপে দেখতে ব্যবহৃত হয়।",
            "main.cpp: 380-383\n(g_animSpeed)"
        ),
        (
            "Key 'K'",
            "এনিমেশন সাময়িক স্থগিত ও পুনরারম্ভ (Pause / Resume Toggle)।",
            "এনিমেশন স্পিডকে ০.০ এবং ১.০-এর মধ্যে সুইচ করে পুরো সিনারিওকে একটি নির্দিষ্ট ফ্রেমে ফ্রিজ করে রাখা যায়।",
            "main.cpp: 385-388\n(g_animSpeed)"
        )
    ]

    for row_idx, rdata in enumerate(data3):
        row = t3.add_row()
        bg_col = "FCF8F5" if (row_idx % 2 == 1) else "FFFFFF"
        for c_idx, text in enumerate(rdata):
            cell = row.cells[c_idx]
            cell.width = col_w3[c_idx]
            set_cell_background(cell, bg_col)
            set_cell_margins(cell, top=80, bottom=80, left=120, right=120)
            p = cell.paragraphs[0]
            p.paragraph_format.space_before = Pt(0)
            p.paragraph_format.space_after = Pt(0)
            run = p.add_run(text)
            run.font.name = 'Calibri'
            run.font.size = Pt(9.5)
            if c_idx == 0:
                run.bold = True
                run.font.color.rgb = RGBColor(0x7A, 0x38, 0x03)

    set_table_borders(t3)

    # ─────────────────────────────────────────────────────────────
    # SECTION 4: ক্যামেরা ভিউ ও সিন প্রিসেট
    # ─────────────────────────────────────────────────────────────
    add_section_header("৪. ক্যামেরা ভিউ ও সিন প্রিসেট (Camera Inspection Presets)")
    add_callout(
        "শোকেসের সময় মাউস দিয়ে খোঁজাখুঁজি না করে সরাসরি ১ থেকে ৯, ০, V, F বা SPACE চেপে স্যারের সামনে নির্দিষ্ট অবজেক্ট বা পুরো গ্রামের সিনেমাটিক দৃশ্য মুহূর্তের মধ্যে প্রদর্শন করা সম্ভব।"
    )

    t4 = doc.add_table(rows=1, cols=4)
    t4.alignment = WD_TABLE_ALIGNMENT.CENTER
    t4.autofit = False
    col_w4 = [Inches(1.0), Inches(2.2), Inches(2.3), Inches(1.27)]
    for j, w in enumerate(col_w4):
        t4.columns[j].width = w

    headers4 = ["কীবোর্ড কি", "প্রিসেট ভিউ ও দৃশ্যের নাম (Preset Scene)", "দৃশ্যমান অবজেক্টসমূহ", "ক্যামেরা টার্গেট কোঅর্ডিনেট"]
    hdr_cells4 = t4.rows[0].cells
    for j, h in enumerate(headers4):
        hdr_cells4[j].width = col_w4[j]
        set_cell_background(hdr_cells4[j], "333366")  # Deep Navy Purple
        set_cell_margins(hdr_cells4[j], top=100, bottom=100, left=120, right=120)
        p = hdr_cells4[j].paragraphs[0]
        p.alignment = WD_ALIGN_PARAGRAPH.CENTER
        run = p.add_run(h)
        run.bold = True
        run.font.name = 'Arial'
        run.font.size = Pt(10)
        run.font.color.rgb = RGBColor(0xFF, 0xFF, 0xFF)

    data4 = [
        (
            "SPACE",
            "সিনেমাটিক ফ্লাই-থ্রু ট্যুর (Cinematic Drone Tour):\n• ২৬ সেকেন্ডের ডায়নামিক লুপ\n• ৯টি ক্যামেরা ওয়েপয়েন্ট",
            "মসৃণ কি-ফ্রেম ইন্টারপোলেশন (LERP) দিয়ে পুরো গ্রামের প্রতিটি বাড়ি, নদী, নৌকা, মসজিদ ও খেতের উপর দিয়ে প্রাণবন্ত ও দ্রুতগতিতে ড্রোন শটের মতো ক্যামেরা উড়ে যায়।",
            "main.cpp: 243-266\nmain.cpp: 335-346"
        ),
        (
            "Key '1'",
            "View 1: মধ্য বাড়ির উঠানের বৈঠক (Courtyard Gathering)",
            "বোনা চারপাই (খাট), হাতে পাখা নিয়ে বসা প্রবীণ মুরব্বি, আড্ডারত গ্রামবাসী, দূরন্ত শিশু, উঠানে মুরগি ও জ্বলন্ত হারিকেন।",
            "Target: (-3.5, 0.75, 1.0)\nDistance: 7.2m"
        ),
        (
            "Key '2'",
            "View 2: নদী ও ঐতিহ্যবাহী নৌকা প্যানোরামা (River & Boats Panorama)",
            "প্রশান্ত নদী, লাল ও সাদা পালতোলা ঐতিহ্যবাহী নৌকা, নদীতে সাঁতারু, জেলে ডিঙি ও দূর দিগন্তের বালুচর। (এখান থেকে 'N' চেপে নৌকা চালানো যায়)।",
            "Target: (8.8, 0.8, 0.5)\nDistance: 21.0m"
        ),
        (
            "Key '3'",
            "View 3: ঐতিহাসিক পোড়ামাটির গ্রামীণ মসজিদ (Gramin Masjid)",
            "পোড়ামাটির ইটের গম্বুজ, লাল বেলেপাথরের ভিত্তিবেদী, সুউচ্চ মিনার এবং মসজিদে গমনরত টুপি-পাঞ্জাবি পরিহিত বৃদ্ধ মুসল্লি।",
            "Target: (-33.0, 1.8, -34.0)\nDistance: 18.0m"
        ),
        (
            "Key '4'",
            "View 4: উত্তর বাড়ি ও গোয়ালঘর (North Farmstead & Cow Shed)",
            "প্রধান দোচালা ঘর, কুটির ২B, ছনের ছাউনি দেওয়া গোয়ালঘর, জাবনা খাওয়ার পাত্র, দুটি দেশি গরু এবং উঠানের খড়ের গাদা।",
            "Target: (-20.5, 1.4, -13.0)\nDistance: 14.5m"
        ),
        (
            "Key '5'",
            "View 5: দক্ষিণ বাড়ি ও কৃষিক্ষেত (South Agricultural Expanse)",
            "দক্ষিণ বাড়ির কুটিরসমূহ (৩, ৩B, ৩C), বাঁশের মাচায় শাকসবজির লতা, ধানের গোলা এবং সাজানো ধানের জমি।",
            "Target: (-11.5, 1.2, 19.5)\nDistance: 16.5m"
        ),
        (
            "Key '6'",
            "View 6: গ্রামীণ বৃক্ষরাজি ও কাশবন (Rural Flora & Riverbank Reeds)",
            "উঁচু নারিকেল ও সুপারি গাছ, কলাবাগান, আমগাছ, ঘন বাঁশঝাড় এবং নদীর পাড়ে বাতাসে দোল খাওয়া সাদা কাশবন।",
            "Target: (1.5, 1.6, -4.5)\nDistance: 14.5m"
        ),
        (
            "Key '7'",
            "View 7: দিন ও রাতের প্রাণীকূল (Diurnal Animal Life Cycle)",
            "রাতে: মুরগির খোপায় (Chicken Coop) নিরাপদে আশ্রিত মুরগি।\nদিনে: উঠানে স্বাধীনভাবে চরে বেড়ানো মুরগি ও নদীর হাঁস।",
            "Target: (-16.0, 0.6, -10.8)\n(Night vs Day Adapt)"
        ),
        (
            "Key '8'",
            "View 8: গ্রামীণ ভূদৃশ্যের পূর্ণাঙ্গ এরিয়াল ভিউ (Grand Full Panoramic View)",
            "সমগ্র গ্রাম এক ফ্রেমে: ৩৩টি কুটির, ১০টি বসতভিটা, নদী, নৌকা, মেঠোপথ, মসজিদ, ফসলের মাঠ ও দূর আকাশের পূর্ণ চাঁদ।",
            "Target: (-3.0, 1.5, 0.0)\nDistance: 48.0m"
        ),
        (
            "Key '9'",
            "View 9: ঐতিহ্যবাহী হালচাষ ক্ষেত্র (Traditional Halchas Field)",
            "জমি তৈরি রত জোড়া বলদ গরু, কাঠের জোয়াল ও লাঙল হাতে কৃষক। (এখান থেকে 'H' চেপে হালচাষ পরিচালনা করা যায়)।",
            "Target: (g_plowX, 1.25, g_plowZ)\nDistance: 8.5m"
        ),
        (
            "Key '0'",
            "View 0: ছনের গোয়ালঘর ও জাবনা খাওয়ার পাত্র (Thatched Cow Shed)",
            "গোয়ালঘরের বিস্তারিত দৃশ্য: বাঁশের খুঁটি, খড়ের ছাদ, জাবনার গামলা ও বিশ্রামরত কুঁজওয়ালা দেশি গরু।",
            "Target: (-25.5, 0.9, -10.5)\nDistance: 8.5m"
        ),
        (
            "Key 'V'",
            "View V: প্যারামেট্রিক মাটির সুরাহি (Bézier Terracotta Pitcher/Surahi)",
            "প্যারামেট্রিক কিউবিক বেজিয়ের কার্ভ দিয়ে তৈরি ঐতিহ্যবাহী পোড়ামাটির সুরাহি ও নিখুঁত অ্যানালিটিক নরমাল সারফেস।",
            "Target: (-20.2, 0.45, 5.2)\nDistance: 3.6m"
        ),
        (
            "Key 'F'",
            "View F: নদীতে খেপলা জাল শিকারী জেলে (Fisherman Hunting Fish)",
            "নদীর পানিতে দাঁড়িয়ে থাকা জেলে, পানিতে ফেলা খেপলা জাল, জাল থেকে লাফিয়ে ওঠা রূপালি মাছ, পোলো ও বাঁশের খালুই।",
            "Target: (fisherX, 0.45, fisherZ)\nDistance: 5.2m"
        )
    ]

    for row_idx, rdata in enumerate(data4):
        row = t4.add_row()
        bg_col = "F8F8FC" if (row_idx % 2 == 1) else "FFFFFF"
        for c_idx, text in enumerate(rdata):
            cell = row.cells[c_idx]
            cell.width = col_w4[c_idx]
            set_cell_background(cell, bg_col)
            set_cell_margins(cell, top=80, bottom=80, left=120, right=120)
            p = cell.paragraphs[0]
            p.paragraph_format.space_before = Pt(0)
            p.paragraph_format.space_after = Pt(0)
            run = p.add_run(text)
            run.font.name = 'Calibri'
            run.font.size = Pt(9.5)
            if c_idx == 0:
                run.bold = True
                run.font.color.rgb = RGBColor(0x33, 0x33, 0x66)

    set_table_borders(t4)

    # ─────────────────────────────────────────────────────────────
    # SECTION 5: সাধারণ ক্যামেরা নেভিগেশন ও মাউস কন্ট্রোল
    # ─────────────────────────────────────────────────────────────
    add_section_header("৫. সাধারণ ক্যামেরা নেভিগেশন ও মাউস কন্ট্রোল (Free Fly-Cam & Mouse Controls)")

    t5 = doc.add_table(rows=1, cols=3)
    t5.alignment = WD_TABLE_ALIGNMENT.CENTER
    t5.autofit = False
    col_w5 = [Inches(1.8), Inches(3.2), Inches(1.77)]
    for j, w in enumerate(col_w5):
        t5.columns[j].width = w

    headers5 = ["কন্ট্রোল ইনপুট (Key / Mouse)", "কাজের বিস্তারিত বিবরণ (Bangla)", "প্রযুক্তিগত বাস্তবায়ন"]
    hdr_cells5 = t5.rows[0].cells
    for j, h in enumerate(headers5):
        hdr_cells5[j].width = col_w5[j]
        set_cell_background(hdr_cells5[j], "444444")
        set_cell_margins(hdr_cells5[j], top=100, bottom=100, left=120, right=120)
        p = hdr_cells5[j].paragraphs[0]
        p.alignment = WD_ALIGN_PARAGRAPH.CENTER
        run = p.add_run(h)
        run.bold = True
        run.font.name = 'Arial'
        run.font.size = Pt(10)
        run.font.color.rgb = RGBColor(0xFF, 0xFF, 0xFF)

    data5 = [
        ("Left Mouse Drag", "ক্যামেরা অরবিট রোটেশন: মাউসের বাম বাটন চেপে ড্র্যাগ করলে ক্যামেরা দৃশ্যের চারপাশে ঘোরে (Yaw ও Pitch পরিবর্তন)।", "cursorPosCallback()\nAngle step: 0.005 rad/px"),
        ("Mouse Scroll Wheel", "ক্যামেরা জুম ইন ও জুম আউট: স্ক্রল আপ করলে অবজেক্টের কাছে যায়, স্ক্রল ডাউন করলে দূরে সরে যায়।", "scrollCallback()\nDistance clamp: [1.2m, 85m]"),
        ("W / S (Fly-Cam Mode)", "ক্যামেরাকে সমান্তরালে সামনে এবং পেছনে এগিয়ে বা পিছিয়ে নিয়ে যাওয়া।", "camFwd += dt * speed"),
        ("A / D (Fly-Cam Mode)", "ক্যামেরাকে সমান্তরালে বামে এবং ডানে স্ট্র্যাফ (Strafe) করে সরানো।", "camRgt += dt * speed"),
        ("E / SPACE (Fly-Cam)", "ক্যামেরার উচ্চতা উপরের দিকে বাড়ানো (Fly Camera Up)।", "camUp += dt * speed"),
        ("Q / C (Fly-Cam)", "ক্যামেরার উচ্চতা নিচের দিকে নামানো (Fly Camera Down)।", "camUp -= dt * speed"),
        ("Shift (Left / Right)", "ক্যামেরা স্পিড বুস্টার: চেপে ধরলে ৩ গুণ গতিতে দ্রুত মুভমেন্ট হয়।", "speedMultiplier = 3.0x"),
        ("ESC", "প্রজেক্ট উইন্ডো বন্ধ ও অ্যাপ্লিকেশন প্রস্থান (Exit Application)।", "glfwSetWindowShouldClose(true)")
    ]

    for row_idx, rdata in enumerate(data5):
        row = t5.add_row()
        bg_col = "F9F9F9" if (row_idx % 2 == 1) else "FFFFFF"
        for c_idx, text in enumerate(rdata):
            cell = row.cells[c_idx]
            cell.width = col_w5[c_idx]
            set_cell_background(cell, bg_col)
            set_cell_margins(cell, top=80, bottom=80, left=120, right=120)
            p = cell.paragraphs[0]
            p.paragraph_format.space_before = Pt(0)
            p.paragraph_format.space_after = Pt(0)
            run = p.add_run(text)
            run.font.name = 'Calibri'
            run.font.size = Pt(9.5)
            if c_idx == 0:
                run.bold = True

    set_table_borders(t5)

    # ─────────────────────────────────────────────────────────────
    # SECTION 6: ভাইভাতে প্যারামিটার পরিবর্তন নির্দেশিকা (Parameter Tweaking Guide)
    # ─────────────────────────────────────────────────────────────
    add_section_header("৬. ভাইভাতে প্যারামিটার পরিবর্তন নির্দেশিকা (Viva Live Code Tweaking)")
    add_callout(
        "স্যার যদি বলেন: 'কোডে অমুক প্যারামিটার পরিবর্তন করে স্ক্রিনে এফেক্ট দেখাও', তখন main.cpp ফাইলের নিচের নির্দিষ্ট লাইনগুলোতে গিয়ে চটজলদি মান পরিবর্তন করবেন:"
    )

    t6 = doc.add_table(rows=1, cols=4)
    t6.alignment = WD_TABLE_ALIGNMENT.CENTER
    t6.autofit = False
    col_w6 = [Inches(1.8), Inches(1.5), Inches(1.8), Inches(1.67)]
    for j, w in enumerate(col_w6):
        t6.columns[j].width = w

    headers6 = ["স্যারের সম্ভাব্য প্রশ্ন / নির্দেশ", "কোড ভ্যারিয়েবল", "ফাইল ও লাইন নম্বর", "কীভাবে পরিবর্তন করবেন"]
    hdr_cells6 = t6.rows[0].cells
    for j, h in enumerate(headers6):
        hdr_cells6[j].width = col_w6[j]
        set_cell_background(hdr_cells6[j], "1B365D")
        set_cell_margins(hdr_cells6[j], top=100, bottom=100, left=120, right=120)
        p = hdr_cells6[j].paragraphs[0]
        p.alignment = WD_ALIGN_PARAGRAPH.CENTER
        run = p.add_run(h)
        run.bold = True
        run.font.name = 'Arial'
        run.font.size = Pt(10)
        run.font.color.rgb = RGBColor(0xFF, 0xFF, 0xFF)

    data6 = [
        (
            "'১ ক্লিকে গরুর গাড়ি আরও বেশি/কম দূরত্ব সরাও'",
            "CART_STEP_DIST\nCART_STEP_ANGLE",
            "main.cpp: 288-289",
            "CART_STEP_DIST = 4.5f; করে দিলে গাড়ি এক ক্লিকে দ্বিগুণ দূরত্বে যাবে।"
        ),
        (
            "'১ ক্লিকে নৌকা আরও বেশি দূরত্বে নিয়ে যাও'",
            "BOAT_STEP_DIST",
            "main.cpp: 306",
            "BOAT_STEP_DIST = 5.0f; করে দিলে প্রতি বৈঠার টানে নৌকা ৫ মিটার এগোবে।"
        ),
        (
            "'১ ক্লিকে হালচাষের স্টেপ বাড়াও বা কমাও'",
            "PLOW_STEP_DIST",
            "main.cpp: 323",
            "PLOW_STEP_DIST = 3.0f; করে দিলে বলদ গরু ও কৃষক আরও দ্রুত এগিয়ে যাবে।"
        ),
        (
            "'টিউবওয়েলের পানি পড়ার সময় বাড়াও'",
            "g_pumpTimer",
            "main.cpp: 349",
            "g_pumpTimer = 7.0f; করে দিলে চাপকলের পানি দীর্ঘক্ষণ প্রবাহিত হবে।"
        ),
        (
            "'লণ্ঠন বা আলোর উজ্জ্বলতা দ্বিগুণ করে দাও'",
            "pointLightIntensity",
            "main.cpp: 1840-1890",
            "পয়েন্ট লাইটের ইনটেনসিটি 1.0f থেকে বাড়িয়ে 2.5f বা 3.0f করে দিলে চারপাশ উজ্জ্বল হবে।"
        ),
        (
            "'শ্যাডিংয়ের চকচকে ভাব (Shininess) পরিবর্তন করো'",
            "shininess\nspecularStrength",
            "Shader.cpp: 38-39\nShader.cpp: 126, 336",
            "shininess = 128.0f; করলে স্পেকুলার হাইলাইট খুব ছোট ও কাচের মতো শার্প হবে।"
        )
    ]

    for row_idx, rdata in enumerate(data6):
        row = t6.add_row()
        bg_col = "F9FBFD" if (row_idx % 2 == 1) else "FFFFFF"
        for c_idx, text in enumerate(rdata):
            cell = row.cells[c_idx]
            cell.width = col_w6[c_idx]
            set_cell_background(cell, bg_col)
            set_cell_margins(cell, top=80, bottom=80, left=120, right=120)
            p = cell.paragraphs[0]
            p.paragraph_format.space_before = Pt(0)
            p.paragraph_format.space_after = Pt(0)
            run = p.add_run(text)
            run.font.name = 'Calibri'
            run.font.size = Pt(9.5)
            if c_idx == 0:
                run.bold = True

    set_table_borders(t6)

    # Footer note
    footer_p = doc.add_paragraph()
    footer_p.paragraph_format.space_before = Pt(16)
    footer_p.paragraph_format.space_after = Pt(0)
    footer_p.alignment = WD_ALIGN_PARAGRAPH.RIGHT
    run_f = footer_p.add_run("— প্রস্তুতকারী: রোল নং ২১০৭০৬৪, কম্পিউটার সায়েন্স অ্যান্ড ইঞ্জিনিয়ারিং বিভাগ, কুয়েট")
    run_f.font.name = 'Calibri'
    run_f.font.size = Pt(9)
    run_f.font.italic = True
    run_f.font.color.rgb = RGBColor(0x66, 0x66, 0x66)

    doc.save(output_path)
    print(f"Document successfully created at: {output_path}")

if __name__ == "__main__":
    out_file = r"c:\Users\HP\source\repos\Hitlar\Project_Keyboard_Controls_Guide_Bangla.docx"
    build_docx(out_file)
