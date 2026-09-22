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

### 2.1 Traditional Wooden Boat (*Dingi Nouka*) & Bengali Hariken
*Source File:* `src/objects/Boat.cpp`

```
                    [Unit Arch] -> Scaled -> Chhoi (Bamboo Hood)
                         |
[Unit Hemisphere] -> Invert Rx(180) -> Scale(0.74, 0.36, 2.40) -> Main Hull Bottom
                         |
           +-------------+-------------+
           |                           |
  [Unit Cone at +Z]           [Unit Cone at -Z]
  Rx(+76°) Sweep              Rx(-76°) Sweep
  Pointed Prow (Bow)          Pointed Stern
           |                           |
  [Procedural Ribs]           [Internal Floor]
  w(z) = 0.5*sqrt(1-(z/2.2)^2)  Recessed Cube(0.42, 0.025, 2.4)
```

1. **Main Hull Bottom (Crescent Shape)**:
   - **Base Primitive:** Unit Upper Hemisphere ($r = 1.0$).
   - **Logic:** Flipped upside-down using $\mathbf{R}_x(180^\circ)$, then non-uniformly scaled by $\mathbf{S}(0.74, 0.36, 2.40)$. This elongates the sphere into an authentic, double-curved, smooth crescent boat hull that is narrow in width ($0.74\text{m}$) and elongated along the water axis ($2.40\text{m}$).
2. **Swept-Up Pointed Bow (Prow)**:
   - **Base Primitive:** Unit Cone ($r=1, h=1$).
   - **Logic:** Positioned at the front joint $Z = +1.80\text{m}$, tilted forward by $\mathbf{R}_x(+76^\circ)$, and scaled by $\mathbf{S}(0.36, 1.15, 0.15)$. The wide base of the cone merges seamlessly into the front hemisphere boundary, while the sharp cone apex extends upward and forward, creating the traditional razor-sharp Bengali prow.
3. **Swept-Up Pointed Stern**:
   - **Base Primitive:** Unit Cone.
   - **Logic:** Positioned at $Z = -1.80\text{m}$, tilted backward by $\mathbf{R}_x(-76^\circ)$, and scaled by $\mathbf{S}(0.36, 1.15, 0.15)$ to produce the symmetrical sweeping stern.
4. **Internal Floorboards**:
   - **Base Primitive:** Unit Cube.
   - **Logic:** Translated down to $Y = 0.04\text{m}$ inside the hull and scaled to $\mathbf{S}(0.42, 0.025, 2.40)$. Because the width is strictly $0.42\text{m}$ (well within the hull's $0.74\text{m}$ beam), the floorboard remains completely recessed inside the hull without clipping outside.
5. **Procedural Curved Cross-Ribs**:
   - **Base Primitive:** Unit Cube.
   - **Mathematical Formulation:** 7 transverse wooden ribs distributed along $Z \in [-1.2\text{m}, +1.2\text{m}]$. At each rib location $z$, the hull width is modeled by the elliptical profile:
     $$w(z) = 0.50 \cdot \sqrt{\max\left(0, 1 - \frac{z^2}{2.2^2}\right)}$$
     Each rib is scaled by $\mathbf{S}(w(z), 0.028, 0.035)$ and translated to $Y = 0.055\text{m}$, perfectly hugging the inner curvature of the hull.
6. **Bamboo Canopy (*Chhoi*)**:
   - **Base Primitive:** Unit Arched Shell.
   - **Logic:** Positioned at $Z = 0.30\text{m}$, elevated to $Y = 0.08\text{m}$, and scaled to $\mathbf{S}(0.74, 0.62, 1.65)$. Framing hoops at the front, middle, and rear are created using slightly larger arches scaled to $(0.755, 0.63, 0.045)$.
7. **Hanging Kerosene Lantern (*Hariken*)**:
   - Suspended under the front arch of the Chhoi at $Z = 1.08\text{m}$ via a vertical wire cylinder ($\mathbf{S}(0.008, 0.08, 0.008)$).

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
2. **Child Sitting Cross-Legged with Book:**
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
     - Unit Triangular Prism scaled to $\mathbf{S}(4.8, 1.5, 3.6)$, providing the quintessential curved Bengali pitched gable profile.
     - Ridge Beam: Unit Cube spanning the entire ridge length $\mathbf{S}(0.12, 0.10, 3.7)$.
4. **Verandah (*Baranda*), Doors & Open Shutters:**
   - Verandah Roof: Unit Cube sloped forward at $12^\circ$ ($\mathbf{S}(4.4, 0.07, 1.35)$).
   - Verandah Pillars: 4 $\times$ Unit Cylinders ($r = 0.07\text{m}$).
   - Windows & Shutters: Open wooden shutters rotated $\pm 45^\circ$ about the vertical hinge axis.
   - Terracotta Water Pitchers (*Matir Kolshi*): Spherical belly ($r = 0.22$) + cylindrical neck ($r = 0.09$) + flared rim cylinder.
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

## 3. Quick Summary Matrix for Viva / Examination

| Object | Primary Starting Primitives | Key Transformations Applied | Unique Procedural Logic |
| :--- | :--- | :--- | :--- |
| **Dingi Nouka** | Unit Hemisphere, Unit Cones, Unit Arch, Unit Cubes | $\mathbf{R}_x(180^\circ)$, non-uniform scale, cone sweeps at $\pm 76^\circ$ | Procedural hull-conforming rib widths $w(z) = 0.5\sqrt{1-(z/2.2)^2}$; recessed floor |
| **Boatman** | Unit Cylinders, Spheres, Cones, Cubes | Hierarchical joint rotations | Conical bamboo *Mathal* hat + long steering oar (*Boitha*) reaching water |
| **Charpai & Fan** | Unit Cylinders, Cubes, Spheres | Coordinate array placement, circumferential loops | Woven jute grid ($8 \times 5$); palm-leaf disk with Nakshi rings and 8 spherical frills |
| **Hariken Lantern** | Unit Cylinders, Spheres, Cones, Arch | Stepped concentric stacking | Emissive flame core; pear-shaped glass globe; twin side draft pipes; wire bail handle |
| **Elder on Charpai** | Unit Cylinders, Spheres, Cones, Cubes | Seated forward kinematics | Distinct forward thighs & hanging draped shins; white cone beard; holds *Haat Pakha* |
| **Standing Villager** | Unit Cylinders, Spheres, Cubes | Compound arm & leg rotations | Two distinct slender leg columns (no single barrel!); center *Kocha* pleat fold |
| **Village Houses** | Unit Cubes, 4-Sided Pyramid, Triangular Prism | Base scaling, eave overhangs | Chouchala (hip roof pyramid) vs. Dochala (pitched prism); raised earthen *Viti* |
| **Clay Stove** | Unit Cylinder, Cones, Sphere | Polar array of prongs | 3 cone prongs supporting spherical cooking pot (*Patil*) with firewood sticks |
| **Hens & Ducks** | Unit Spheres, Cones, Cubes | Non-uniform scale, axial rotations | Ellipsoidal bodies; cone beaks/tails; flattened duck bill vs. hen comb |
| **Coconut Palm** | Unit Cylinders, Spheres, Cubes | 8-segment iterative stack with angle increment | Continuous trunk curvature; 3-segment cascading frond kinematic chains |
| **Banana Tree** | Unit Cylinders, Spheres, Cubes | 3-stage paddle droop chains | Arching broad paddle blades; hanging bunch with purple flower heart (*Mocha*) |
| **Mango Tree** | Unit Cylinders, Spheres, Cubes | 4-way branch radiation, multi-cluster spheres | Organic non-uniform canopy formed by 8 overlapping ellipsoidal leaf clusters |
| **Bamboo Grove** | Unit Cylinders, Cubes | Multi-stalk cluster with nodal rings | Clustered segmented stalks with annular culm joints every $0.7\text{m}$ |
| **Paddy Field** | Unit Cylinders, Cones | 5-way radial spread with droop | Rice tillers with drooping golden grain panicles (*Dhaner Shish*) |
