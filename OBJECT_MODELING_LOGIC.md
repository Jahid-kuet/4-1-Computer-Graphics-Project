# Procedural 3D Object Modeling: Geometric Construction & Derivation Guide
**Course:** Computer Graphics & Image Processing Laboratory (CSE 4100 / KUET)  
**Project Title:** Village Gathering by the River – A Bangladeshi Rural Night Scene in 3D  
**Roll No:** 2107064  
**Graphics API:** OpenGL 3.3 Core Profile (GLSL 330 core, C++17)

---

## 1. Foundational Theory & Modeling Pipeline

### 1.1 The Mathematical Primitives (Canonical Unit Geometries)
In strict accordance with computer graphics laboratory requirements, **zero external 3D models (.obj, .fbx) are imported**. Every object in the scene is constructed procedurally using canonical mathematical primitives generated at startup in `Primitives.cpp`:

1. **Unit Cube**: Defined over the coordinate range $[-0.5, +0.5]^3$, centered at origin $(0, 0, 0)$ with dimensions $1 \times 1 \times 1$.
2. **Unit Cylinder**: Centered along the $Y$-axis with radius $r = 1.0$ and height $h = 1.0$, extending from $Y = -0.5$ to $Y = +0.5$.
3. **Unit Sphere**: Centered at origin $(0, 0, 0)$ with radius $r = 1.0$ generated via latitude-longitude tessellation.
4. **Unit Cone**: Base of radius $r = 1.0$ resting on the $XZ$-plane ($Y = 0$) with apex pointed at $(0, 1, 0)$.
5. **Unit Upper Hemisphere**: Half-sphere of radius $r = 1.0$ ($Y \ge 0$), open at bottom.
6. **Unit Arched Shell**: Extruded semi-cylindrical arch of outer radius $0.5$, inner radius $0.42$, and length $1.0$ along the $Z$-axis.
7. **Unit 4-Sided Pyramid**: Square base $1 \times 1$ at $Y = 0$ with apex at $(0, 1, 0)$.
8. **Unit Triangular Prism**: Equilateral triangular cross-section of width $1.0$ and height $1.0$ in $XY$, extruded $1.0$ along $Z$.

---

### 1.2 Affine Transformation Pipeline
Any procedural part is positioned, rotated, and sized by constructing a $4 \times 4$ Model Transformation Matrix $\mathbf{M}$:

$$\mathbf{M} = \mathbf{T}(t_x, t_y, t_z) \cdot \mathbf{R}_y(\theta_y) \cdot \mathbf{R}_x(\theta_x) \cdot \mathbf{R}_z(\theta_z) \cdot \mathbf{S}(s_x, s_y, s_z)$$

* **Scale ($\mathbf{S}$)**: Stretches/flattens the unit dimensions $(1, 1, 1)$ to the target physical dimensions (e.g., length, radius, thickness).
* **Rotate ($\mathbf{R}$)**: Orients the component along the designated local axis.
* **Translate ($\mathbf{T}$)**: Positions the component relative to its parent joint or origin.

### 1.3 Hierarchical Modeling (Parent-Child Kinematic Chains)
Complex objects (such as humans with articulated limbs, the boat with seated boatman and oar, or palm trees with cascading fronds) utilize hierarchical matrix stacks:

$$\mathbf{M}_{\text{child}} = \mathbf{M}_{\text{parent}} \cdot \mathbf{T}_{\text{joint}} \cdot \mathbf{R}_{\text{joint}} \cdot \mathbf{T}_{\text{offset}} \cdot \mathbf{S}_{\text{limb}}$$

This ensures that moving or rotating a parent (e.g., the boat rocking on water, or the elder's shoulder rotating) automatically carries along all attached children (the boatman, oar, lantern, forearm, and hand fan).

---

## 2. Detailed Object-by-Object Geometric Construction Logic

---

### 2.1 Traditional Wooden Sailboat (*Pal Tola Dingi Nouka*) & Bengali Hariken
*Source File:* `src/objects/Boat.cpp`

```
               [Bamboo Yard Spar (Dondi)] (W = 2.30m at Y = 3.08m)
                           |
       +-------------------+-------------------+
       |                                       |
[Bamboo Mast (Mastul)]                 [Billowing Sail (Pal)]
h = 3.20m, 6 Nodal Rings               2.10m x 1.75m Cotton Cloth
       |                               Belly Depth Dz = 0.38m + Wind Flutter
       |                                       |
       |                               [Terracotta Folk Stripe]
       |                               Midsection Accent Band
       +-------------------+-------------------+
                           |
     [Woven Bamboo Chhoi Canopy & 4 Hoops] with Hanging Hariken (Z = 0.72m)
                           |
     [Flared Timber Sheer Hull] (Length 5.20m, Beam 1.04m, Sheer 0.20m -> 0.57m)
       |                   |                   |
[Sculpted Golui Beak]  [5x Thwarts]    [Swept Pachha Stern]
Brass Patti & Eye      Mast Collar &   Elevated Gura Seat
(Noukar Chokh)         7 Curved Ribs   for Seated Boatman
```

1. **Continuous Flared Timber Sheer Hull & Rocker**:
   - **Authentic Vernacular Geometry:** Real country boats (*Dingi Nouka*) are characterized by a long, graceful sheer line that curves upward from midships ($Y = 0.20\text{m}$) towards the bow ($Y = 0.57\text{m}$) and stern ($Y = 0.47\text{m}$).
   - **Mathematical Formulation:** Generated along $36$ longitudinal stations ($Z \in [-2.55\text{m}, +2.65\text{m}]$) and $16$ transverse girth slices ($u \in [-\pi/2, +\pi/2]$):
     $$w(z) = 0.52 \cdot \sqrt{\max\left(0, 1 - z_c^2\right)} \cdot (1 - 0.15 z_c^2)$$
     $$y_{sheer}(z) = 0.20 + 0.30 z_c^2 + 0.05 z_c, \quad y_{keel}(z) = -0.16 + 0.26 z_c^2$$
     $$x(z, u) = w(z) \sin(u), \quad y(z, u) = y_{keel}(z) + [y_{sheer}(z) - y_{keel}(z)] \cdot (1 - \cos(u))$$
   - **Watertight Solid Timber Planking:** Dual-surface mesh with outer hull strakes and inner hull planks offset by $0.016\text{m}$ along exact outward vertex normals, capped along both gunwales with smooth timber rub-rails.

2. **Sculpted Bow Beak (*Golui / গলুই*) with Auspicious Eye (*Noukar Chokh*)**:
   - Swept-up forward beak extending to $Z = +2.75\text{m}$ ($Y = 0.54\text{m}$) with a sharp cutwater keel.
   - Polished brass protective banding cap plate (*Golui Patti*) in brass gold (`vec3(0.85, 0.72, 0.25)`).
   - Auspicious folk boat eyes (*Noukar Chokh*) on port and starboard prow flanks: ivory sclera almond sphere, gloss black pupil, and crimson center dot, celebrated in rural Bengal to "guide the boat through foggy monsoon river channels".

3. **Swept Stern (*Pachha*) & Elevated Boatman Seat (*Gura*)**:
   - Sternpost sweeping upward to $Y = 0.44\text{m}$ at $Z = -2.55\text{m}$ with brass finial cap.
   - Elevated teak stern thwart (*Pachhar Gura*) at $Z = -1.30\text{m}$, $Y = 0.14\text{m}$, ergonomically supporting the seated Boatman (*Majhi*) holding the steering oar.

4. **Internal Framing Ribs (*Bata / Dara*), Thwarts (*Goor*) & Floorboards (*Patan*)**:
   - 5 heavy timber cross-beams tying the gunwales together, including a reinforced square mast-step collar at $Z = 1.15\text{m}$.
   - 7 transverse curved wooden framing ribs hugging the flared inner hull contour.
   - Recessed longitudinal timber deck planks spanning the bottom floor.

5. **Iconic Arched Bamboo Canopy (*Chhoi / ছই*) & Hanging Hariken**:
   - Woven split-bamboo arched tunnel shell ($1.30\text{m}$ long, $0.92\text{m}$ wide) spanning $Z \in [-0.65\text{m}, +0.65\text{m}]$.
   - 4 bamboo arch hoops and 3 longitudinal bamboo purlin battens reinforcing the roof ridge and flanks.
   - Vintage Dietz-style tubular Hurricane Lantern (*Hariken*) suspended by a wire bail under the forward arch hoop at $Z = 0.72\text{m}$.

6. **Iconic Billowing Sail (*Pal / পাল*) & Rigging System**:
   - **Bamboo Mast (*Baash-er Mastul*):** Stepped at $Z = 1.15\text{m}$ into the reinforced thwart, rising $3.20\text{m}$ into the sky to $Y = 3.34\text{m}$, with 6 dark bamboo nodal rings and masthead pulley truck.
   - **Yard Spar (*Dondi*):** Horizontal bamboo spar spanning $2.30\text{m}$ across the top of the sail at $Y = 3.08\text{m}$, plus a lower boom spar at $Y = 1.28\text{m}$.
   - **Aerodynamic Billowing Sail:** Large $2.10\text{m} \times 1.75\text{m}$ cloth sail with deep forward billow belly catching the wind:
     $$\Delta z(x, y) = 0.38 \cdot \sin\left(\pi \frac{y - 1.30}{1.75}\right) \cdot \cos\left(\frac{\pi x}{2.10}\right)$$
     Double-sided shaded unbleached cotton/khadi cloth with dynamic summer breeze flutter.
   - **Terracotta Folk Stripe & Corner Patches (*Kona Patti*):** Traditional reddish-ochre folk horizontal stripe band across the midsection, and 4 reinforced corner patches.
   - **Authentic Rigging Ropes (*Kasi / Rashi*):** Forestay running to bow Golui, port and starboard shroud stays securing the mast to the gunwales, lower sheet lines (*Kasi*) tying the sail corners to the boat's thwarts, and halyard lashings.

---

### 2.2 Boatman (*Majhi*) with Conical Bamboo Hat (*Mathal*) & Oar (*Boitha*)
*Source File:* `src/objects/Boatman.cpp`

1. **Hierarchical Seated Human Pose**:
   - Root matrix positioned at the stern seat of the boat ($Z = -1.30\text{m}, Y = 0.16\text{m}$).
   - Torso, head, arms, and legs constructed using the anatomical human rig (detailed in Section 2.3).
2. **Traditional Bamboo Sunhat (*Mathal*)**:
   - **Base Primitive:** Unit Cone.
   - **Logic:** Translated to the top of the head ($Y = 0.685\text{m}$), rotated with a slight rakish angle $\mathbf{R}_x(8^\circ)$, and scaled to $\mathbf{S}(0.32, 0.14, 0.32)$.
   - **Rim Trim:** Unit Cylinder scaled to $\mathbf{S}(0.33, 0.02, 0.33)$ forming the woven outer split-bamboo hoop.
3. **Long Steering Oar (*Boitha*)**:
   - **Hierarchical Pivot:** Established beside the boatman's hands resting across the starboard gunwale at $\mathbf{T}(0.26, 0.26, 0.12)$ with compound rotation $\mathbf{R}_x(-38^\circ + \theta_{\text{sway}}) \cdot \mathbf{R}_z(18^\circ)$.
   - **Shaft:** Unit Cylinder child of pivot, scaled to $\mathbf{S}(0.025, 1.60, 0.025)$ and translated downward along its local axis.
   - **Oar Paddle Blade:** Unit Cube child of pivot, translated to the tip of the shaft and flattened to $\mathbf{S}(0.16, 0.38, 0.02)$, entering the river water.

---

### 2.3 Traditional Charpai, Seated Elder & Handmade Palm-Leaf Fan (*Haat Pakha*)
*Source Files:* `src/objects/Charpai.cpp`, `src/objects/Person.cpp`

```
[4x Unit Cylinders] -> Legs (h=0.5m, r=0.04m)
[4x Unit Cubes]     -> Frame Rails (1.2m x 2.0m)
[8x5 Unit Cubes]    -> Criss-Cross Jute Weave Matrix
         |
    [Seated Elder] (Kurta/Panjabi + Maroon Lungi + White Beard)
         |
    [Right Shoulder Joint] -> Rx(-22°)
         |
    [Right Elbow Joint]    -> Rx(-65°), Ry(-18°) [Raised forward]
         |
    [Right Hand Sphere]    -> Firm palm grip
         |
    [Haat Pakha (Hand Fan)]
         +-- Handle: Unit Cylinder (r=0.012, h=0.32)
         +-- Palm-Leaf Blade: Unit Cylinder (r=0.18, t=0.008)
         +-- Outer Rim Hoop: Unit Cylinder (r=0.186, t=0.012)
         +-- Nakshi Embroidery: Concentric Rings (crimson & green)
         +-- 8x Frills (ঝালর): Unit Spheres distributed at perimeter
```

1. **Charpai Structure**:
   - **4 Legs:** 4 $\times$ Unit Cylinders of height $0.50\text{m}$ and radius $0.04\text{m}$ positioned at $(\pm 0.55\text{m}, \pm 0.95\text{m})$.
   - **Outer Frame:** 4 $\times$ Unit Cubes scaled along edges: two long side rails $\mathbf{S}(0.05, 0.05, 2.0)$ and two end rails $\mathbf{S}(1.2, 0.05, 0.05)$.
   - **Woven Jute Bedding:** Interlocking grid of $8 \times 5$ thin Unit Cubes ($\mathbf{S}(1.08, 0.015, 0.03)$ transverse and $\mathbf{S}(0.03, 0.015, 1.80)$ longitudinal) slightly recessed below the wooden rails.
2. **Seated Elder (Purely Geometric & Explainable Primitives)**:
   - **Torso:** 1 Unit Cube scaled to $\mathbf{S}(0.30, 0.42, 0.18)$ (width, height, depth) representing the cotton kurta body.
   - **Head & Neck:**
     - Neck: 1 Unit Cylinder scaled to $\mathbf{S}(0.04, 0.05, 0.04)$.
     - Head: 1 Unit Sphere scaled to $\mathbf{S}(0.11, 0.11, 0.11)$.
     - *Zero hair and zero beard mesh overlays* — strictly canonical, explainable shapes.
   - **Gamcha:** 1 Unit Cube folded over the right shoulder ($\mathbf{S}(0.065, 0.44, 0.38)$).
   - **Seated Lower Body:**
     - Thighs: Two Unit Cubes ($\mathbf{S}(0.052, 0.055, 0.25)$) extending horizontally forward over the charpai edge.
     - Shins: Two Unit Cubes ($\mathbf{S}(0.048, 0.21, 0.052)$) hanging vertically downward into sandaled feet cubes.
3. **Handmade Palm-Leaf Fan (*Haat Pakha*) — Upward/Above Direction**:
   - Attached to the Elder's right hand matrix and oriented pointing **UPWARD**:
   - **Handle:** Slender Unit Cylinder scaled to $\mathbf{S}(0.012, 0.32, 0.012)$ held in the palm.
   - **Blade:** Flattened Unit Cylinder disk ($\mathbf{S}(0.18, 0.18, 0.008)$) extending straight **UPWARD** ("above direction") in front of the chest.
   - **Outer Rim Hoop:** Unit Cylinder hoop ($\mathbf{S}(0.186, 0.186, 0.012)$).
   - **Nakshi Embroidery:** Concentric crimson and green rings.
   - **Frills (ঝালর):** 8 Unit Spheres placed circumferentially around the perimeter.

---

### 2.4 Traditional Backdated Kerosene Lantern / Kupi
*Source File:* `src/objects/Charpai.cpp` (`drawLantern`)

Designed strictly from pure fundamental geometric shapes for a backdated, rustic village appearance:
1. **Oil Tank Base:** 1 Unit Cylinder scaled to $\mathbf{S}(0.10, 0.08, 0.10)$ in dark tin/clay.
2. **Burner Neck:** 1 Unit Cylinder scaled to $\mathbf{S}(0.048, 0.030, 0.048)$ in brass.
3. **Glowing Flame:** 1 Unit Cone scaled to $\mathbf{S}(0.024, 0.075, 0.024)$ rendered with emissive glow.
4. **Glass Chimney:** 1 Unit Cylinder scaled to $\mathbf{S}(0.065, 0.14, 0.065)$ in clear translucent glass.
5. **Tin Smoke Cap:** 1 Unit Cone scaled to $\mathbf{S}(0.088, 0.045, 0.088)$ capping the top.
6. **Bail Handle:** 1 Unit Arch scaled to $\mathbf{S}(0.15, 0.14, 0.012)$ spanning over the top.

*Identical backdated lantern model is used in both locations:*
- On the low stool in front of the Charpai
- Hanging under the bamboo Chhoi on the Dingi Nouka

---

### 2.5 Standing Villager & Child Reading Book
*Source File:* `src/objects/Person.cpp`

1. **Standing Villager (Two Distinct Leg Columns in Lungi):**
   - **Problem Solved:** Traditional student models often use a single bulky cylinder for the entire lower body, creating a barrel-like look.
   - **Our Geometric Formulation:**
     - Central Pelvis drape: Unit Cylinder $\mathbf{S}(0.126, 0.12, 0.08)$ anchoring waist to legs.
     - Two distinct slender leg columns: Two Unit Cylinders scaled to $\mathbf{S}(0.046, 0.43, 0.050)$ placed at $X = \pm 0.062\text{m}$.
     - Center front pleat (*Kocha* fold): Slender vertical Unit Cube $\mathbf{S}(0.022, 0.41, 0.014)$ running down the midline.
     - Ankle columns: Unit Cylinders ($r = 0.024\text{m}, h = 0.09\text{m}$) in skin color.
     - Sandaled feet: Unit Cubes ($\mathbf{S}(0.036, 0.026, 0.082)$).
2. **Respectful Standing Arm Kinematics & Anatomical Hands (*Haat Badha*):**
   - **Problem Solved:** Vertically hanging arms with elongated spherical hands at hip level created an awkward, ambiguous silhouette near the groin from 3/4 camera angles.
   - **Kinematic Formulation:**
     - Upper Arm: Slender kurta sleeve cylinders ($r = 0.027\text{m}, h = 0.24\text{m}$) hanging naturally alongside the torso with forward pitch $\mathbf{R}_x(-12^\circ)$ and inward roll $\mathbf{R}_z(\mp 5^\circ)$.
     - Internal Shoulder Rotation: $\mathbf{R}_y(\mp 36^\circ)$ internally rotates the shoulder joint to orient the elbow hinge across the chest.
     - Elbow Flexion Hinge: $\mathbf{R}_x(-68^\circ)$ swings the forearm diagonally across the lower abdomen at navel height ($Z \approx +0.16\text{m}$), completely clear of the crotch and hips.
     - Respectful Hand Overlap (*Haat Badha*): Right hand rests gently over the left wrist in front of the lower kurta/navel.
     - Sculpted Hands (Zero Spherical Ambiguity): Flat palm box ($\mathbf{S}(0.020, 0.038, 0.030)$), forward-curled finger block ($\mathbf{S}(0.018, 0.026, 0.028)$ rotated $15^\circ$), and flush thumb box ($\mathbf{S}(0.016, 0.022, 0.010)$) tucked along the inner radial contour.
3. **Child Sitting Cross-Legged with Book:**
   - Proportional scale factor $0.68 \times$ applied to adult human hierarchy.
   - **Cross-Legged Lap:** Horizontal Unit Cube $\mathbf{S}(0.19, 0.10, 0.28)$ across pelvis.
   - **Crossed Shins:** Two Unit Cylinders rotated $88^\circ$ around $X$ and $\pm 75^\circ$ around $Y$, resting flat on the courtyard.
   - **Bengali Book:** Unit Cube scaled to $\mathbf{S}(0.35, 0.035, 0.26)$ tilted $25^\circ$ forward in hands.

---

### 2.6 Authentic Rural Houses (*Chouchala* & *Dochala*) & Clay Stove (*Matir Chula*)
*Source File:* `src/objects/House.cpp`

1. **Raised Earthen Plinth (*Viti / Dawa*):**
   - Unit Cube scaled to $\mathbf{S}(4.4, 0.25, 4.2)$ forming the elevated packed-earth base that protects rural mud homes from monsoon rainwater. Front entrance steps built from an additional Unit Cube.
2. **Sun-Dried Mud Walls:**
   - Central Room: Unit Cube scaled to $\mathbf{S}(3.6, 2.0, 2.8)$.
   - Corner Timber Posts: 4 $\times$ Unit Cubes ($\mathbf{S}(0.12, 2.05, 0.12)$) anchoring the four exterior corners.
3. **Traditional Roof Systems (Zero Cone Roofs):**
   - **Chouchala (House 1 - 4-Sloped Hip Roof):**
     - Unit 4-Sided Pyramid scaled to $\mathbf{S}(4.8, 1.6, 4.0)$ with generous thatch eaves extending $0.6\text{m}$ beyond the walls.
     - Crest Ridge Cap: Unit Cube along the central peak $\mathbf{S}(1.9, 0.10, 0.16)$.
   - **Dochala (House 2 - 2-Sloped Pitch Gable Roof):**
     - Dimensions: Long chala frontage length $3.6\text{m}$ along $Z$, gable width $2.8\text{m}$ along $X$, wall height $2.0\text{m}$.
     - Unit Triangular Prism scaled to $\mathbf{S}(4.0, 1.5, 4.4)$, providing the authentic pitched gable profile with $0.6\text{m}$ front/back eave overhangs and $0.4\text{m}$ gable overhangs.
     - Ridge Beam: Unit Cube spanning the entire ridge length $\mathbf{S}(0.12, 0.10, 4.5)$.
     - Front Entrance Arrangement: In authentic vernacular Bengal architecture, the front entrance door, 4-pillar bamboo verandah (*Baranda*), and front windows are placed on the **long chala side** ($+X$, under the sloping eave), while the triangular gable walls ($+Z / -Z$) form the side ends with an authentic shuttered ventilation window.
4. **Verandah (*Baranda*), Doors & Open Shutters:**
   - Verandah Roof: Unit Cube sloped forward at $12^\circ$ ($\mathbf{S}(1.35, 0.07, 4.4)$ along the long chala side).
   - Verandah Pillars: 4 $\times$ Unit Cylinders ($r = 0.07\text{m}$) spaced along the long chala frontage.
   - Wooden Entrance Door & Frame: $0.70\text{m} \times 1.40\text{m}$ centered on the front wall.
   - Windows & Shutters: Open wooden shutters rotated $\pm 45^\circ$ about the vertical hinge axis.
   - Terracotta Water Pitchers (*Matir Kolshi*): Spherical belly ($r = 0.22$) + cylindrical neck ($r = 0.09$) + flared rim cylinder placed on the earthen verandah plinth.
5. **Outdoor Clay Cooking Stove (*Matir Chula*):**
   - Cylindrical mud base $\mathbf{S}(0.42, 0.22, 0.38)$.
   - 3 $\times$ Unit Cones arranged at $120^\circ$ intervals forming the clay prongs that support cookware.
   - Cooking Pot (*Hari / Patil*): Dark Unit Sphere $\mathbf{S}(0.18, 0.13, 0.18)$.
   - Firewood Sticks: Angled Unit Cylinders extending into the hearth opening.

---

### 2.7 Domestic Animals: Village Hen & River Duck
*Source Files:* `src/objects/Hen.cpp`, `src/objects/Duck.cpp`

1. **Village Hen:**
   - **Body:** Unit Sphere non-uniformly scaled to $\mathbf{S}(0.12, 0.11, 0.16)$ (compact, plump oval).
   - **Wings:** Two Unit Cubes placed on flanks, rotated $\pm 12^\circ$ roll and scaled to $\mathbf{S}(0.025, 0.075, 0.12)$.
   - **Head & Comb:** Spherical head ($r = 0.06$) topped with bright red comb sphere $\mathbf{S}(0.02, 0.035, 0.045)$.
   - **Beak:** Unit Cone rotated $90^\circ$ around $X$ pointing forward ($\mathbf{S}(0.016, 0.045, 0.018)$).
   - **Tail Feathers:** Unit Cone pointing upward and backward at $-45^\circ$ ($\mathbf{S}(0.04, 0.13, 0.06)$).
   - **Legs & Claws:** Thin Unit Cylinders ($r = 0.012$) and flat Unit Cube feet.
2. **River Duck:**
   - **Body:** Unit Sphere scaled to $\mathbf{S}(0.28, 0.20, 0.35)$ (wider, flatter buoyant profile for floating on water).
   - **Head:** Unit Sphere ($r = 0.11$) in characteristic mallard green.
   - **Bill:** Unit Cone rotated $90^\circ$ and flattened to $\mathbf{S}(0.06, 0.10, 0.03)$, capturing the wide, spatulate duck bill.
   - **Tail:** Small Unit Cone pointing upward at $-50^\circ$.

3. **Diurnal vs. Nocturnal Presence & Traditional Chicken Coop (*Murgir Khopa / মোরগের খোঁপা*)**:
   - **Nocturnal Animal Gating:** In authentic rural Bengal, domestic poultry and waterfowl are sheltered inside coops before dusk to protect them from nocturnal predators (jackals, wildcats, mongooses). Consequently, in the **Night Scene** (`lightingMode == 0`), free-range hens and river ducks are **not present** roaming outside in the courtyard or river.
   - **Daytime Roaming:** During daytime modes (`lightingMode == 1, 2, 3`), 5 free-range hens actively roam, peck, and waddle across the homestead courtyards, while 4 ducks swim exploratory loops with water bobbing and paddling pitch along the river shallows.
   - **Village Chicken Coop (*Murgir Khopa*):** Procedural bamboo coop elevated on 4 short stilts ($h = 0.36\text{m}$) placed in the homestead yard beside House 1 on the earthen ground (completely off the family living verandah). At night, its front wooden door is latched shut with an iron pin; by day, the door is hinged downward as a walking ramp to the yard.
   - **Interactive Animal Inspection Preset (`Key 7`):** Pressing `Key 7` automatically switches the lighting mode to daytime so that users can immediately inspect the animated hens and ducks.

---

### 2.8 Indigenous Trees & Agricultural Vegetation
*Source Files:* `src/objects/Tree.cpp`, `src/objects/Terrain.cpp`

1. **Coconut Palm (*Narikel Gach*):**
   - **Curved Trunk:** Kinematic chain of 8 segmented Unit Cylinders ($h = 0.75\text{m}$ each). At each segment, the local frame advances upward, tapers its radius ($r_i = 0.16 - 0.05 \cdot \frac{i}{8}$), and applies a subtle $1.8^\circ$ rotation around $Z$. This integrates to an organic, continuous, graceful curved palm trunk.
   - **Annular Leaf Scars:** Thin flared cylinder rings ($r \times 1.15, h = 0.035$) marking each segment boundary.
   - **Coconuts:** 6 green Unit Spheres nestled in a ring at the crown.
   - **Cascading Fronds:** 10 fronds spaced at $36^\circ$. Each frond is a 3-segment compound kinematic arm:
     - Segment 1 (Base): Unit Cylinder spine + flat Unit Cube leaflets tilted $30^\circ$.
     - Segment 2 (Mid): Unit Cylinder spine + flat Unit Cube leaflets arched outward an additional $32^\circ$.
     - Segment 3 (Tip): Slender Unit Cube drooping steeply downward an additional $38^\circ$.
2. **Banana Tree (*Kola Gach*):**
   - **Pseudostem:** Succulent green Unit Cylinder ($\mathbf{S}(0.18, 2.4, 0.18)$).
   - **Broad Paddle Leaves:** 7 radiating paddle leaves, each composed of 3 sequential bending rectangular plates ($28^\circ \to 42^\circ \to 48^\circ$ droop).
   - **Blossom & Fruit Bunch:** Curved hanging stem cylinder + 5 miniature cylinder bananas + pointed purple Unit Sphere (*Mocha* / banana heart).
3. **Banyan / Mango Tree (*Bot / Aam Gach*):**
   - **Trunk & Buttress Roots:** Massive central Unit Cylinder ($r = 0.40, h = 2.8$) braced by 4 angled Unit Cube root buttresses.
   - **Primary Limbs:** 4 Unit Cylinders radiating outward and upward at $38^\circ$.
   - **Tiered Canopy:** 8 overlapping, scaled Unit Spheres ranging from $r = 1.4\text{m}$ to $r = 2.6\text{m}$ arranged in clusters to create an organic, leafy canopy without repeating geometric shapes.
4. **Bamboo Grove (*Bansher Jhar*):**
   - Clump of 8 tall segmented culm cylinders ($h = 4.2\text{m} \sim 5.5\text{m}, r = 0.045\text{m}$) each with distinct lean angles. Dark node rings placed every $0.7\text{m}$ along the height, topped by angled leaflet cubes.
5. **Rice Plants (*Dhan Gachh*) in Paddy Fields:**
   - Each clump (*Gochha*) consists of 5 outward-radiating green Unit Cylinders tilted $14^\circ \sim 18^\circ$, topped by drooping golden Unit Cones representing ripe grain panicles (*Dhaner Shish*).

---

### 2.9 Historic Terracotta Village Mosque (*ঐতিহাসিক প্রাচীন গ্রামীণ পোড়ামাটির মসজিদ*)
*Source File:* `src/objects/Mosque.cpp`

```
                      [Lotus Kalasa Spire & Ghata]
                                   |
                [Central Terracotta Dome] (r=1.80, h=1.45)
                                   |
             [Stepped Octagonal Drum & Stucco Ring Band]
                                   |
           [Curved Bengal Sultanate Cornice (Dochala Parapet)]
                                   |
        [Ancient Terracotta Brick Hall] (5.4m x 3.3m x 5.0m)
   +-------------------------------+-------------------------------+
   |                               |                               |
[4x Octagonal Corner Towers]   [Multi-Cusped Arched Portal]   [Paka Houz Cistern]
Burj with Kumbha rings & dome  Recessed Sal Doors & Transom   Brick Tank & 3 Clay Bodnas
```

1. **Raised Weathered Brick Plinth & Stepped Choukath (*Viti*)**:
   - Unit Cube scaled to $\mathbf{S}(7.2, 0.35, 7.8)$ in weathered red terracotta brick. 3 tiered entrance steps with rounded bullnose edges lead worshippers up to the front entrance.
2. **Main Prayer Hall with Curved Bengal Cornice (*Dochala Parapet*)**:
   - Central hall $\mathbf{S}(5.0, 3.3, 5.0)$ in rich terracotta masonry (`brickMain`), divided into horizontal registers with lime-surki mortar string courses.
   - Parapet features the signature curved roof cornice of Bengal Sultanate architecture, computed via a parabolic crown function $y(x) = \text{crownH} \cdot (1 - (2x/W)^2)$ with tangent-rotated brick dentil blocks and molded stucco drip-molds along both front and rear facades.
3. **Four Sturdy Octagonal Corner Buttress Towers (*বুরুজ - Burj*)**:
   - 4 robust medieval octagonal masonry towers flanking the four corners of the hall.
   - Divided into 3 vertical registers by molded terracotta belt rings (*Kumbha* bands) and lime stucco string courses.
   - Overhanging bracketed cornice crowning each tower, topped by ribbed terracotta cupolas (*Chhatri*) with lotus crest finials and antique brass spires.
   - Front-left tower is elevated as the historic village Azaan platform.
4. **Central Grand Terracotta Brick Dome (*Boro Gombuj*)**:
   - Transition drum: octagonal cylindrical drum base $\mathbf{S}(1.75, 0.56, 1.75)$ with decorative lime mortar frieze and stepped lotus collar (*Padma Pith*).
   - Grand hemispherical dome $\mathbf{S}(1.80, 1.45, 1.80)$ in burnt clay terracotta brick with concentric molded terracotta ribbing rings.
   - Multi-tiered antique brass finial (*Kalasa*): inverted lotus base, terracotta round water-pot (*Ghata*), tapered spire cone, and golden apex sphere.
5. **Multi-Cusped Terracotta Pointed Entrance Portal (*খিলান তোরণ*)**:
   - Authentic projecting entrance bay (*Pishtaq*) with flanking terracotta piers, plinth base moldings, and arch-springing impost capitals.
   - Outer multi-cusped pointed terracotta arch ring and inner recessed arch soffit framing carved terracotta rosettes in the spandrels.
   - Deeply recessed Sal timber double doors set in a heavy timber frame (*Choukath*), reinforced with forged iron strap hinges, studs, meeting stile, and antique brass ring handles.
   - Arched transom above the wooden doors featuring openwork terracotta lattice (*Jali*) radiating warm golden prayer sanctuary light.
   - Hanging brass kerosene lantern (*Hariken*) suspended by a wrought-iron chain from the arch apex.
   - Flanking arched oil-lamp niches (*Chiragdan*) with warm glowing clay lamps.
6. **Terracotta Jali Openwork Windows (*পোড়ামাটির জালি বাতায়ন*)**:
   - Arched window bays with molded brick casings, projecting sills, and pointed capping arches.
   - Perforated terracotta lattice grid (4 vertical $\times$ 5 horizontal bars) with warm interior amber prayer sanctuary light.
7. **Projecting Semi-Octagonal Western Mehrab Bay (*মেহরাব*)**:
   - Authentic Sultanate projection on the western rear Qibla wall ($\mathbf{S}(1.65, 2.40, 0.65)$), crowned with a curved cornice and decorative mini cupola.
8. **Masonry Ablution Cistern (*Paka Houz*) & Clay Bodnas (*মাটির বদনা*)**:
   - Open brick-paved courtyard (*Uthan*) with a raised red brick ablution cistern and clean water surface.
   - Brick seating bench equipped with 3 procedurally modeled handcrafted clay ablution ewers (*Matir Bodna*), each featuring a bulbous belly, tapered neck, flanged rim, curved loop handle, and upward-angled pouring spout.

---

### 2.10 Rural Homestead Elements: Cow Shed (*Gowal Ghor*) & Straw Stacks (*Khorer Paloi*)
*Source File:* `src/objects/House.cpp`

1. **Traditional Thatched Cow Shed (*Gowal Ghor*)**:
   - **Open Bamboo Structure:** 4 corner bamboo post cylinders ($h = 1.70\text{m}, r = 0.065\text{m}$) supporting an open-sided shelter.
   - **Dochala Thatched Roof:** Unit Triangular Prism scaled to $\mathbf{S}(4.0, 1.10, 3.1)$.
   - **Feeding Trough (*Chari*):** Hollowed timber bin filled with golden straw and fodder.
   - **Side Railings:** Bamboo half-wall slats on three sides.
2. **Traditional Rice Straw Stacks (*Khorer Paloi / খড়ের পালা*)**:
   - **Base Stilts:** Low stone/brick support footings protecting straw from wet monsoon soil.
   - **Lower Body:** Cylindrical packed golden rice straw $\mathbf{S}(0.85, 1.10, 0.85)$.
   - **Pointed Thatch Cap:** Conical rain-shedding cap $\mathbf{S}(0.96, 1.35, 0.96)$.
   - **Central Bamboo Pole (*Baash-er Khuti*):** Slender vertical bamboo pole driven through the core, protruding through the top apex to brace the stack against high winds.

---

### 2.11 Expanded Village Road Network & River Landing Ghat (*Nodi-r Ghat*)
*Source Files:* `src/objects/Terrain.cpp`, `src/objects/River.cpp`

1. **Main Village Road (*Gramin Boro Rasta*)**:
   - Continuous unpaved earthen road winding through the village territory from north to south ($Z = -26\text{m} \to +26\text{m}$), connecting all homestead baris and the mosque.
2. **Branching Footpaths (*Poth*)**:
   - River Ghat Road: curves down from the central courtyard to the river water's edge.
   - Mosque Road: brick-soling path with terracotta hue leading to the mosque entrance.
   - Homestead paths connecting north bari, south bari, and terraced paddy field dikes (*Aal*).
3. **River Landing Ghat (*Nodi-r Ghat*)**:
   - Heavy vertical bamboo/timber piles driven into the riverbed with coiled jute mooring rope.
   - Stepped timber deck platforms descending into the water, providing docking for Dingi boats and an authentic rural riverside landing.

---

---

### 2.12 Bioluminescent Fireflies (*Jonaki Poka / জোনাকি পোকা*) & Nocturnal Multi-Light System
*Source Files:* `src/objects/Fireflies.cpp`, `src/objects/Stars.cpp`, `src/Shader.cpp`, `main.cpp`

1. **Bioluminescent Jonaki Poka (*জোনাকি পোকা*)**:
   - **Population & Ecological Clustering:** 160 procedural fireflies distributed deterministically across six rural ecological micro-habitats:
     * *Bamboo Grove & Orchard Cluster:* hovering among dense dark foliage ($X \in [-13.5, -5.0]$, $Z \in [-17.5, -7.0]$).
     * *Riverbank, Reeds & Water Lilies:* skimming low over flowing water and catkin plumes ($X \in [4.2, 8.2]$, $Z \in [-16.0, 16.0]$).
     * *Courtyard Gathering & Charpai:* dancing near sitting elders, children reading books, and bamboo fences.
     * *Terraced Paddy Fields:* shimmering along the earthen *Aal* dikes and rice stalks.
     * *Mosque Courtyard & Date Palms:* fluttering near stone steps and ablution basin.
     * *Canopy Sky Drift:* drifting lazily between palm tree tops and the open night sky.
   - **3D Lissajous Flight Dynamics:** Each firefly executes smooth, non-linear harmonic 3D trajectories:
     $$x(t) = x_0 + r_x \cdot [0.65\sin(\omega t + \phi) + 0.35\cos(1.63\omega t)]$$
     $$y(t) = y_0 + r_y \cdot [0.70\sin(1.37\omega t + 1.4\phi) + 0.30\cos(2.81\omega t)]$$
     $$z(t) = z_0 + r_z \cdot [0.65\cos(0.91\omega t + 0.7\phi) + 0.35\sin(2.19\omega t)]$$
   - **Enzymatic Luciferase Flash & Nocturnal Blink Cycle:**
     * **Warm Sun-Yellow / Solar Golden Palette:** Rendered with pure warm sun-yellow colors ($\text{RGB}(1.0, 0.92, 0.16)$, $\text{RGB}(1.0, 0.85, 0.08)$, $\text{RGB}(1.0, 0.96, 0.26)$) with zero blue or green tints.
     * **Nocturnal Blinking Mechanism:** Fireflies remain dark/dormant for ~75% of their cycle ($T \approx 1.8\text{s} - 3.4\text{s}$), winking ON with sharp, brilliant sun-yellow flashes ($\sin^{1.5}$) during their active flash window.
     * **Species Variation:** 70% single-flash blinkers and 30% rapid double-pulse blinkers ("wink-wink"), creating a natural rhythmic dance of glowing sun sparks across the village night.
     * **Day/Night Gating:** Active strictly during the moonlit night (`lightingMode == 0`); during daytime modes, fireflies and celestial bodies remain inactive.

2. **Nocturnal 4-Point Light & Moonlight Pipeline**:
   - **Directional Moonlight:** Silvery-blue cool moonbeams ($\text{RGB}(0.65, 0.75, 0.95)$) cast along vector $(0.47, -0.68, 0.57)$ from the Full Moon at $(-18, 26, -22)$, producing Blinn-Phong specular glints across the river and roofs.
   - **Point Light 1 (Courtyard Hurricane Lantern):** Warm amber flame ($\text{RGB}(1.0, 0.72, 0.28)$) with organic high-frequency flicker illuminating the Charpai, elders, and swept earthen courtyard.
   - **Point Light 2 (Boat Hurricane Lantern):** Floating amber lantern on the moored Dingi Nouka with river-bobbing vertical translation.
   - **Point Light 3 (Mosque Entrance Mehrab Lantern):** Warm brass lantern glow ($\text{RGB}(1.0, 0.78, 0.35)$) illuminating the 3-step stone plinth, arched entrance portal, and brick road.
   - **Point Light 4 (Clay Cooking Stove Fire):** Flickering orange-red wood ember glow ($\text{RGB}(1.0, 0.42, 0.10)$) radiating over the clay stove and kitchen hut wall.
   - **Twinkling Sky Dome:** 140 emissive star spheres with individual sinusoidal brightness oscillation $\sin(\omega_i t + \phi_i)$.

---

### 2.13 Traditional Hand-Pump Tubewell (*Chapa Kol / টিউবওয়েল*) & Terracotta Water Pitchers (*Kolshi*)
*Source Files:* `src/objects/House.cpp`, `main.cpp`

1. **Village Tubewell (*টিউবওয়েল / Chapa Kol*)**:
   - **Raised Concrete Washing Pad (*Pacca Tala*):** Rectangular concrete slab $\mathbf{S}(1.60, 0.12, 1.60)$ with safety perimeter curb lips and a dark wet circular puddle under the pump spout.
   - **Drainage Trough (*Pani-r Nala*):** Sloped concrete channel directing excess wash water towards the nearby banana tree grove.
   - **Cast-Iron Pump Barrel:** British-racing rural green cylinder ($r = 0.12, h = 0.75$) with bolted mounting base flange and middle reinforcing band.
   - **Spout (*Mukhi*) & Head:** Forward-projecting horizontal cylinder with curved discharge lip, top cylinder head, and vertical plunger rod.
   - **Curved Handle (*Hatol*):** Lever pivoted from the pump neck extending backward with a counterweight sphere on the tip.
   - **Terracotta Kolshi:** Water pitcher placed directly under the spout on the wet concrete pad.

2. **Clay Water Pitchers (*Matir Kolshi / মাটির কলসি*)**:
   - **Lower Belly:** Spherical terracotta clay bulb $\mathbf{S}(0.34, 0.28, 0.34)$ in rich baked-earth hue `RGB(0.68, 0.34, 0.16)`.
   - **Base Footing:** Darker burnt clay ring $\mathbf{S}(0.16, 0.04, 0.16)$ providing stability.
   - **Tapered Neck & Flared Lip:** Narrow cylindrical throat with a flared rolled mouth rim.
   - Placed at the tubewell, beside the outdoor clay cooking stove, on the house verandah (*Dawa*), and at the river landing ghat.

---

### 2.14 Procedural Deshi Cow (*দেশি গরু*) & Riverbank Fishing Net Drying Rack
*Source Files:* `src/objects/House.cpp`, `src/objects/River.cpp`

1. **Procedural Deshi Cow (*দেশি গরু*)**:
   - **Torso & Hump (*Kud*):** Ellipsoidal barrel body $\mathbf{S}(0.75, 0.68, 1.45)$ in warm fawn-brown with cream underbelly; distinct Zebu shoulder hump $\mathbf{S}(0.38, 0.42, 0.48)$ situated over the front withers.
   - **Neck & Dewlap (*Golakomblo*):** Upward-angled neck cylinder with a draped skin fold underneath the throat.
   - **Head & Horns:** Tapered muzzle with dark nostrils, gentle dark eyes, downward-tilted floppy ears, and a pair of curved ivory cones pointing upward and inward.
   - **Folded Haunches & Tail:** Tucked resting forelegs and rounded hindquarters resting on straw bedding in the cow shed (*Gowal Ghor*); long slender tail with dark switch draped over the flank.

2. **Fishing Net Drying Rack (*Jal Shukanor Macha*)**:
   - Two upright bamboo stilt poles with annular nodes driven into the sandy riverbank.
   - Long horizontal bamboo ridge beam ($L = 4.2\text{m}$).
   - Draped dark braided fishing net mesh hanging down in scalloped catenary folds with sinker fringe.
   - Slender bamboo boat push-pole (*Logi*) propped up against the rack.

---

### 2.15 Procedural Motion, Dynamic Kinematics & Interactive Simulation Engine
*Source Files:* `main.cpp`, `src/objects/House.cpp`, `src/objects/River.cpp`

1. **River Channel Navigation & Dynamic Rowing Kinematics (Boat 2 & Boatman)**:
   - **Continuous River Voyage:** The gliding Dingi Nouka traverses a 42-meter stretch of the meandering river ($Z \in [-22.0, +20.0]\text{m}$) using modular time translation:
     $$Z(t) = -22.0 + \text{fmod}(t \cdot v_{nav},\; 42.0)$$
   - **Meander Centerline Tracking:** Procedurally computes the exact curved channel position from the river's mathematical spline:
     $$X(Z) = 8.0 + 1.8 \sin(0.08 Z + 0.4) + 0.6 \cos(0.04 Z) + 0.5$$
   - **Curvature Tangent Steering:** The boat steers into the river current by computing the analytic derivative:
     $$\theta_{yaw}(Z) = \text{atan2}\left(\frac{dX}{dZ},\; 1.0\right)$$
   - **Synchronized Rowing Propulsion Surge:** Each stroke of the boatman's oar (*Boitha*) imparts a forward acceleration surge $\Delta Z = \sin(\omega t) \cdot 0.045\text{m}$, accompanied by dipping oar kinematics ($\pm 16^\circ$), pitch trim ($-\cos(\omega t) \cdot 1.5^\circ$), roll rocking ($\pm 1.8^\circ$), and water heave bobbing ($\sin(2\omega t) \cdot 0.022\text{m}$).

2. **Autonomous Courtyard Hens (Free-Range Foraging, Wall Collision Avoidance & Waddling Gait)**:
   - **Open-Yard Foraging Orbits:** 5 free-range hens (*Deshi Murgi*) wander distinct swept earthen courtyard sub-regions (central uthan, coop forecourt, west courtyard, foreground yard, north farmyard) via parametric trajectories:
     $$X_h(t) = X_0 + r_x \sin(\omega_h t + \phi_h), \quad Z_h(t) = Z_0 + r_z \cos(\omega_h t + \phi_h)$$
   - **Active Obstacle & Home Wall Collision Avoidance:** An analytic AABB obstacle collision resolution system evaluates all 10 village architectural boundaries (House 1, House 2 Dochala, House 5, Kitchen Hut, Cow Shed, Cooking Stove, Chicken Coop, Tubewell Pad, Charpai, and Courtyard Perimeter Fence), clamping each hen outside building plinths with a safety margin ($r = 0.18\text{m}$) and dynamically turning its heading tangent away from walls, completely preventing hens from penetrating or crossing any walls.
   - **Tangential Heading Alignment:** Automatically faces forward along velocity vector $\mathbf{v} = (\dot{X}, \dot{Z})$:
     $$\theta_{heading} = \text{atan2}(\dot{X}_h, \dot{Z}_h)$$
   - **Waddling Locomotion vs. Pecking:** During walking segments, body rolls in a side-to-side waddle gait ($\pm 3.8^\circ$ at $9\text{Hz}$); upon pausing, the hen dips its head and beak down to the ground in rapid grain pecking cycles ($\pm 16^\circ$ at $8.5\text{Hz}$).

3. **River Ducks Exploratory Swimming Paths**:
   - 4 ducks (*Pati Hash*) navigate smooth elliptical water eddies along river shallows and Kashbon reed beds.
   - Yaw smoothly aligns with instantaneous swimming velocity vector; water buoyancy heave ($\pm 0.022\text{m}$) and paddling pitch rocking ($\pm 2.2^\circ$) simulate realistic fluid immersion.

4. **Vegetation Aerodynamics (Summer Breeze Wind Sway)**:
   - Foliage harmonic oscillation simulates rural evening river breezes when enabled (`Key B`):
     - Coconut palm crowns sway with compound dual-axis rotation ($\pm 2.5^\circ$).
     - Broad banana paddle leaves flutter softly ($\pm 1.8^\circ$ at $2.1\text{Hz}$).
     - Bamboo culms bend flexibly in wind gusts ($\pm 2.4^\circ$).
     - Large Mango tree canopies exhibit gentle breathing mass displacement ($\pm 1.4^\circ$).

5. **Interactive Hand-Pump Tubewell Mechanics (`Key P`)**:
   - Activating tubewell hand-pumping drives a reciprocal angular oscillation ($\pm 20^\circ$) of the curved cast-iron handle and plunger rod.
   - Groundwater flows dynamically from the cylindrical spout: a translucent cyan water jet cascades downward into the neck of the terracotta *Kolshi*, producing an animated expanding splash puddle ring on the wet concrete pad.

6. **Automated Cinematic Fly-Through Tour Mode (`SPACE`)**:
   - 7 camera keyframes smoothly navigate the camera target, distance, yaw, and pitch over a 59.0-second loop using smoothstep cubic interpolation:
     $$s(t) = t^2 (3 - 2t)$$
   - Tour guides the viewer through: (1) River Dingi & Rowing Boatman, (2) Landing Ghat & Net Drying Racks, (3) Swept Courtyard, Elders, Children & Tubewell, (4) Village Mosque with Emerald Dome, (5) North Homestead & Thatched Cow Shed, (6) Terraced Paddy Fields, Dancing Jonaki & Moon, and (7) Grand Village Panorama.
   - Any manual mouse drag or keyboard input instantly and seamlessly yields control back to the user.

7. **Hariken Lantern Flame Control (`Key H`) & Animation Speed Controls (`[`, `]`, `K`)**:
   - 4 selectable lantern flame intensities: Normal Golden Glow ($1.0\times$), Extra Bright Blazing Flame ($1.85\times$), Soft Amber Glow ($0.45\times$), and Extinguished ($0.0\times$ for pure moonlit night inspection).
   - High-frequency chaotic harmonic flame flicker modulates point light radius and specular highlights.
   - Real-time variable animation time scale ($0.25\times$ slow-motion to $3.0\times$ fast-forward), with single-key animation pause and resume (`Key K`).

---

### 2.16 Multi-Source Lighting Architecture (Directional & 6 Positional Point Lights)
*Source Files:* `src/Shader.cpp`, `main.cpp`, `src/objects/Terrain.cpp`

The scene implements a complete, textbook-grade Blinn-Phong lighting model combining a global **Directional Light** with **6 Positional Point Lights**:

1. **Directional Light (Moonlight / Sunlight)**:
   - **Parallel Ray Illumination:** Constant vector $\mathbf{L}_{dir}$ across all world coordinates with zero distance attenuation.
   - **Ambient, Diffuse & Blinn-Phong Specular:**
     $$\mathbf{I}_{dir} = K_a \mathbf{C}_{L} + \max(\mathbf{N} \cdot \mathbf{L}_{dir}, 0) \mathbf{C}_{L} + K_s (\mathbf{N} \cdot \mathbf{H}_{dir})^\alpha \mathbf{C}_{L}$$
     where $\mathbf{H}_{dir} = \text{normalize}(\mathbf{L}_{dir} + \mathbf{V})$.
   - **Independent Interactive Toggle (`Key J`):** Allows students and examiners to turn the directional moonlight completely OFF to inspect point light attenuation patterns and falloff circles in pure nocturnal darkness.

2. **Positional Point Lights (6 Distinct Light Sources with Quadratic Attenuation)**:
   - Each point light computes radial distance $d = \|\mathbf{P}_{light} - \mathbf{P}_{frag}\|$, normalized incident direction $\mathbf{L}_p$, half-vector $\mathbf{H}_p$, and standard quadratic attenuation:
     $$\text{att}(d) = \frac{\text{window}(d)}{K_c + K_l \cdot d + K_q \cdot d^2}$$
     with smooth Hermite windowing $\text{window}(d) = \left(\text{clamp}\left(1 - \frac{d^2}{R_{max}^2}, 0, 1\right)\right)^2$.
   - **The 6 Configured Point Lights in the Village:**
     * **Point Light 1: Courtyard Hurricane Lantern (*Hariken*)** at $(-2.60, 0.425, 0.60)$: Warm kerosene flame ($1.0, 0.72, 0.28$) illuminating the Charpai, seated elder, listening children, and earthen uthan ($R_{max} = 24.0\text{m}$).
     * **Point Light 2: Moored Boat Hurricane Lantern** at $(7.0, 0.55, 2.28)$: Amber lantern hanging from the Dingi's bamboo canopy (*Chhoi*), casting specular highlights across river ripples ($R_{max} = 18.0\text{m}$).
     * **Point Light 3: Mosque Portal Mehrab Lamp** at $(-14.0, 2.45, 6.75)$: Warm brass arched lamp illuminating the mosque entrance, steps, and minaret base ($R_{max} = 20.0\text{m}$).
     * **Point Light 4: Kitchen Clay Cooking Stove Fire (*Matir Chula*)** at $(-6.20, 0.35, -6.60)$: Fiery red-orange embers ($1.0, 0.42, 0.10$) with dual-harmonic chaotic flicker inside the earthen stove ($R_{max} = 15.0\text{m}$).
     * **Point Light 5: River Landing Ghat Mooring Post Lantern** at $(4.70, 0.70, 0.00)$: Hanging lantern on the wooden ghat piling illuminating the bamboo gangway, water steps, and fishing net racks ($R_{max} = 18.0\text{m}$).
     * **Point Light 6: Cruising Dingi Nouka Dynamic Lantern** at $(X_{boat}(t), Y_{boat}(t), Z_{boat}(t))$: Moves in real time along with the rowing boat down the winding river, dynamically casting moving light circles across the water surface, boatman, and riverbank ($R_{max} = 18.0\text{m}$).

3. **Master Interactive Lighting Controls**:
   - `Key J`: Toggle Directional Light (Moonlight/Sunlight) ON/OFF
   - `Key H`: Cycle 6 Point Lights (Normal $1.0\times$ $\to$ Bright $1.85\times$ $\to$ Soft Amber $0.45\times$ $\to$ OFF)
   - `Key L`: Cycle 4 Global Lighting Environments (Moonlit Night, Bright Day, Unlit Facets, Pure Flat)

---

### 2.17 Procedural Textures & Parametric Bézier Curved Objects
*Source Files:* `include/Texture.h`, `src/Texture.cpp`, `include/objects/CurvedObject.h`, `src/objects/CurvedObject.cpp`, `src/Shader.cpp`

In accordance with strict computer graphics lab standards, **zero external image files** are imported. Instead, a procedural texture generator and mathematically rigorous curved surface models are implemented directly in C++ and OpenGL 3.3 Core Profile:

```
                            [Cubic Bézier Profile Curve B(u)]
                                  u in [0, 1], 4 Control Points
                                             |
                                  [Revolve 360° around Y]
                                  theta in [0, 2pi], 36 sectors
                                             |
                         [Analytic Surface Normal Derivation]
                         N = dS/d_theta x dS/du = (y' cos, -r', y' sin)
                                             |
                   +-------------------------+-------------------------+
                   |                                                   |
      [Bézier Terracotta Surahi / Vase]              [Curved Arched Bamboo Footbridge]
      32 vertical slices x 36 radial sectors         Parabolic Arch Y(z) = H*(1 - (2z/L)^2)
      Dedicated GPU VAO/VBO/EBO Mesh                 24 Curved Cross-Slats, Double Handrails
```

#### 1. Procedural 2D GPU Texture Generator (`Texture.cpp`)
Generates 4 distinct $256 \times 256$ RGBA textures uploaded to the GPU via `glTexImage2D` with full mipmapping (`glGenerateMipmap`) and repeat wrapping:
1. **Wood Grain (`TEX_WOOD`)**: Concentric distorted tree growth rings and fine longitudinal wood fiber streaks creating seasoned Bengali Sal/Teak timber.
2. **Clay Brick (`TEX_BRICK`)**: Running-bond terracotta clay bricks with mortar joints and kiln color variations.
3. **Bamboo Mat / Thatch (`TEX_BAMBOO`)**: Traditional diagonal woven bamboo split strips (*Chatai*) with interwoven fiber grain.
4. **Checked Lungi Fabric (`TEX_FABRIC`)**: Authentic rural Bengali checked lungi/gamcha plaid pattern with intersecting dark navy, forest green, madder red, and white pinstripes.

**Interactive Texture Mode Toggle (`Key X`):**
* **Mode 0: Solid Shading** — Clean Blinn-Phong geometry.
* **Mode 1: Procedural GLSL Detailing** — Real-time GPU micro-grain computed dynamically per fragment.
* **Mode 2: GPU Texture Maps** — Sampled from the procedural $256 \times 256$ RGBA textures.

#### 2. Parametric Cubic Bézier Surface of Revolution (Terracotta Surahi / Vase)
Modeled by evaluating a cubic Bézier profile curve $(r(u), y(u))$:
$$\mathbf{B}(u) = (1-u)^3 P_0 + 3(1-u)^2 u P_1 + 3(1-u) u^2 P_2 + u^3 P_3$$
with control points defining the classic Bengali *Matir Surahi*:
* $P_0 = (0.28, 0.00)$: Base rim
* $P_1 = (0.82, 0.38)$: Bulging spherical body
* $P_2 = (0.16, 0.78)$: Slender tapered neck
* $P_3 = (0.36, 1.00)$: Flared mouth rim

**Analytic Surface Normal:**
$$\frac{\partial \mathbf{S}}{\partial u} = \left(\frac{dr}{du}\cos\theta, \frac{dy}{du}, \frac{dr}{du}\sin\theta\right), \quad \frac{\partial \mathbf{S}}{\partial\theta} = (-r\sin\theta, 0, r\cos\theta)$$
$$\mathbf{N}(u, \theta) = \frac{\partial \mathbf{S}}{\partial\theta} \times \frac{\partial \mathbf{S}}{\partial u} = \text{normalize}\left(\frac{dy}{du}\cos\theta, -\frac{dr}{du}, \frac{dy}{du}\sin\theta\right)$$
32 vertical parameter steps $\times$ 36 radial sectors yield 2,304 smooth triangles uploaded to a dedicated GPU VAO/VBO/EBO.

#### 3. Traditional Curved Arched Bamboo Footbridge (*Bansher Saako* / বাঁশের সাঁকো)
Spanning $7.6\text{m}$ across the river inlet with a parabolic arch profile:
$$Y(z) = Y_{base} + H_{arch}\left(1 - \left(\frac{2z}{L}\right)^2\right), \quad \text{Slope Angle } \theta(z) = \arctan\left(-\frac{8 H_{arch} z}{L^2}\right)$$
* **3 Curved Longitudinal Runners:** Bamboo culms following the arch curve.
* **24 Curved Cross-Slat Tread Planks:** Tangential to the parabolic arch with natural alternating color variations.
* **Double Curved Handrails & 14 Upright Stanchions:** Matching the deck curve with jute rope lashings.
* **4 Pairs of A-Frame Diagonal Stilts:** Cross-braced pilings driven into the riverbed.
* **Camera Preset (`Key V`):** Focuses camera directly on the Curved Footbridge and Bézier Surahi.

---


### 2.18 Broader View Panoramic Composition (Matching User Concept Artwork)
*Source Files:* main.cpp, src/objects/Terrain.cpp, src/objects/Fireflies.cpp, src/objects/Mosque.cpp

A dedicated, elevated cinematic camera preset (Key 8 and default startup view) was engineered to frame the entire rural Bengali riverine landscape exactly as depicted in the concept artwork:

1. **Camera Framing & Geometric Projection**:
   - **Target Point:** $\mathbf{T} = (0.50, 1.20, -6.50)$ (looking north-northeast down the village road and river bend).
   - **Orbit Angles:** $\text{Yaw} = -12.0^\circ$, $\text{Pitch} = +14.0^\circ$, $\text{Distance} = 19.5\text{m}$.
   - **Eye Vantage Position:** $\mathbf{E} = (-3.43, 5.92, 12.01)$, offering an elevated bird's-eye perspective over the swept homestead courtyard, river channel, and horizon.
   - In screen Normalized Device Coordinates (NDC):
     * Foreground Courtyard & Charpai: $ (lower-left quadrant).
     * Reading Children & Lantern: $ (bottom-center courtyard).
     * Roaming Hens & Courtyard Ducks: $ (lower-left ground).
     * Standing Zebu Cow (*Deshi Goru*): $ (courtyard fence border).
     * Historic Village Mosque: $ (middle-ground river bend).
     * Winding River Channel: $ (stretching across the right half of the screen).
     * Radiant Full Moon: $ (upper-right night sky).

2. **Luminous Full Moon & Silvery River Moonbeam Trail**:
   - **Moon Position:** Placed high in the upper-right night sky at 18 10 -28$, casting cool silvery illumination across the water.
   - **Directional Moonlight Vector:** $\mathbf{L}_{dir} = \text{normalize}(-0.35, -0.48, 0.80)$.
   - **Specular Water Trail:** The Blinn-Phong specular reflection $\mathbf{R} = 2(\mathbf{N} \cdot \mathbf{L})\mathbf{N} - \mathbf{L}$ off the horizontal river water plane $ aligns with the camera view vector $\mathbf{V}$, producing a continuous shimmering moonbeam reflection trail glistening down the river ripples directly toward the viewer.

3. **Spatial Village Realignment (North River Bend Mosque)**:
   - The ancient terracotta Sultanate mosque is repositioned to -3.8 0 -17.5$ along the curving northern riverbank, with its arched entrance portal, 4 octagonal corner towers, and grand hemispherical dome visible along the winding earthen road.
   - Point Light 3 illuminates the terracotta facade from -3.8 2.28 -14.42$.
   - The brick-soling access road connects the main village road cleanly to the mosque plinth steps.

4. **Authentic Spatial Village Object Distribution & Decongested Courtyard Gathering**:
   - **Courtyard Decongestion:** Auxiliary objects previously clustered inside a tiny 4-meter central courtyard radius were redistributed to their authentic functional rural zones across the expanded homesteads:
     * **Standing Zebu Cow (*Deshi Goru*):** Moved from the congested central yard to the North Homestead farmyard at $(-15.5, 0, -5.2)$ beside the thatched Cow Shed (*Gowal Ghor*) and primary Straw Stack.
     * **Waterfowl (*Pati Hash* Pair):** Moved from the courtyard floor behind the reading children to the lush riverbank grass near the River Landing Ghat at $(4.2, 0, 0.4)$ and $(4.5, 0, 0.7)$, preening beside the water channel.
     * **Parametric Bézier Terracotta Vases (*Surahi*):** Shifted from the tubewell pad to the front entrances of East-Bari House 5 at $(-12.5, 0.08, 1.8)$ and South Homestead House 3 at $(-6.2, 0.08, 13.8)$, highlighting architectural thresholds.
     * **Terracotta Water Pitchers (*Matir Kolshi*):** Distributed across 5 realistic village usage points: Tubewell concrete apron $(-4.6, 0, -2.1)$ for water fetching; Dochala House 2 yard $(-11.8, 0, -6.2)$; Outdoor Kitchen Hut $(-4.8, 0, -8.6)$ for cooking; River Ghat platform $(4.8, 0.12, 1.8)$ for washing; and South Homestead House 3 $(-6.8, 0, 14.2)$.
     * **Foreground Straw Stack 3:** Positioned at $(-6.8, 0, 4.8)$ along the outer perimeter bamboo fence, framing the foreground camera shot without crowding the courtyard seating.
   - **Courtyard Gathering Spacing & Ergonomics:**
     * **Charpai & Elder:** Placed comfortably at $(-4.2, 0, 0.6)$ rotated $-15^\circ$.
     * **Hariken Lantern on Stool:** Center point light source at $(-3.0, 0.40, 0.6)$, providing $1.2\text{m}$ clear focal distance in front of the elder.
     * **Neighbor on Bench:** Angled at $50^\circ$ at $(-5.0, 0.16, 1.8)$, $1.4\text{m}$ from the Charpai with generous elbow room.
     * **Standing Villager:** Naturally stationed along the entrance path at $(-1.8, 0, -1.2)$ rotated $-55^\circ$, enjoying over $2.0\text{m}$ open clearance.
     * **Reciting Child & Attentive Sibling:** Positioned opposite each other in the warm lantern glow at $(-2.4, 0, 1.8)$ and $(-1.6, 0, 2.2)$, with clear open pathways all around.
   - **Sun-Yellow Jonaki (Fireflies):** 160 warm-yellow fireflies clustered densely along the foreground bamboo fence, courtyard bushes, and riverbank.

---

## 3. Quick Summary Matrix for Viva / Examination

| Object | Primary Starting Primitives | Key Transformations Applied | Unique Procedural Logic |
| :--- | :--- | :--- | :--- |
| **Pal Tola Dingi Nouka** | Continuous Sheer Lofting, Billowing Sail Grid, Unit Arch, Cylinders, Cubes, Spheres | Longitudinal stations with flared concave-convex strakes, aerodynamic forward billow | Flared sheer hull (0.20m to 0.57m); sculpted Golui beak with brass Patti & auspicious eye (Noukar Chokh); Pachha stern with Gura seat; Chhoi bamboo hood & Hariken; 3.2m bamboo mast with billowing cotton Pal & rigging |
| **Boatman** | Unit Cylinders, Spheres, Cones, Cubes | Hierarchical joint rotations | Conical bamboo *Mathal* hat + long steering oar (*Boitha*) reaching water |
| **Village Mosque** | Unit Cubes, Hemisphere, Cylinders, Cones, Arch | Curved parabolic cornice, octagonal buttress arrays, multi-cusped arch portal | Ancient Sultanate terracotta brick masjid; Dochala curved parapet; 4 octagonal corner towers; central dome with lotus Kalasa; Sal timber double doors & Paka Houz with 3 clay Bodnas |
| **Village Houses** | Unit Cubes, 4-Sided Pyramid, Triangular Prism | Base scaling, eave overhangs | Chouchala (hip roof) vs. Dochala (curved gable); raised earthen *Viti* & bamboo verandah |
| **Cow Shed & Cow** | Unit Cylinders, Prism, Spheres, Cones, Cubes | Compound stacking, anatomical hierarchy | Thatched open cow shed with hay trough & procedural resting humped Zebu cow (*Deshi Goru*) |
| **Tubewell & Kolshi** | Unit Cylinders, Cubes, Spheres | Stepped cylindrical stacking, flared rims | Classic green cast-iron pump with handle, concrete washing pad, drainage trough & terracotta *Kolshi* |
| **Bézier Terracotta Vase** | Parametric Bézier Surface of Revolution | Cubic curve evaluation $B(u)$ revolved $360^\circ$ around $Y$ | 32 vertical slices $\times$ 36 sectors; exact analytic normals $\mathbf{N} = (y'\cos\theta, -r', y'\sin\theta)$ |
| **Bamboo Footbridge** | Parabolic Curve, Unit Cylinders, Cubes | Arch profile $Y(z) = H(1-(2z/L)^2)$ with tangent rotation | 3 curved runner poles, 24 cross-slats, curved double handrails, and cross-braced stilts |
| **Procedural Textures** | 2D GPU Texture Generator (4 Maps) | $256 \times 256$ RGBA procedural generation with mipmaps | Wood grain, Clay brick, Bamboo mat/thatch, Checked lungi plaid; real-time toggle `Key X` |
| **Fishing Net Rack** | Unit Cylinders, Cubes | Bamboo stilts, draped mesh scaling | Riverbank bamboo scaffold with draped nylon-jute net mesh & propped boat push-pole (*Logi*) |
| **Jonaki (Fireflies)** | Unit Spheres (Emissive) | Luminous scale pulse, 3D Lissajous translation | 160 warm sun-yellow flies clustered across village; periodic nocturnal blinking with double-pulse winks |
| **Charpai & Fan** | Unit Cylinders, Cubes, Spheres | Coordinate array placement, circumferential loops | Woven jute grid ($8 \times 5$); palm-leaf disk with Nakshi rings and 8 spherical frills |
| **Hariken Lantern** | Unit Cylinders, Spheres, Cones, Arch | Stepped concentric stacking | Emissive flame core; pear-shaped glass globe; twin draft pipes; bail handle |
| **Elder on Charpai** | Unit Cylinders, Spheres, Cones, Cubes | Seated forward kinematics | Distinct forward thighs & hanging draped shins; white cone beard; holds *Haat Pakha* |
| **Standing Villager** | Unit Cylinders, Spheres, Cubes | Compound arm & leg rotations | Two distinct slender leg columns (no single barrel!); center *Kocha* pleat fold |
| **River Landing Ghat** | Unit Cubes, Cylinders | Stepped descent stacking | Heavy timber pilings with coiled jute rope; tiered landing planks into water |
| **Road Network** | Unit Planes | Waypoint trajectory segmentation | Continuous spline-like segmented earthen & brick roads connecting all baris |
| **Clay Stove** | Unit Cylinder, Cones, Sphere | Polar array of prongs | 3 cone prongs supporting spherical cooking pot (*Patil*) with firewood sticks |
| **Hens & Ducks** | Unit Spheres, Cones, Cubes | Non-uniform scale, axial rotations | Ellipsoidal bodies; cone beaks/tails; flattened duck bill vs. hen comb |
| **Coconut Palm** | Unit Cylinders, Spheres, Cubes | 8-segment iterative stack with angle increment | Continuous trunk curvature; 3-segment cascading frond kinematic chains |
| **Banana Tree** | Unit Cylinders, Spheres, Cubes | 3-stage paddle droop chains | Arching broad paddle blades; hanging bunch with purple flower heart (*Mocha*) |
| **Mango Tree** | Unit Cylinders, Spheres, Cubes | 4-way branch radiation, multi-cluster spheres | Organic non-uniform canopy formed by 8 overlapping ellipsoidal leaf clusters |
| **Bamboo Grove** | Unit Cylinders, Cubes | Multi-stalk cluster with nodal rings | Clustered segmented stalks with annular culm joints every $0.7\text{m}$ |
| **Paddy Field** | Unit Cylinders, Cones, Planes, Cubes | Grid array, perimeter boundary dikes | Terraced paddy plots enclosed by raised *Aal* dikes with golden grain panicles |

