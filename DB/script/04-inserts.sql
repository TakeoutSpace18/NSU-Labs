-- Departments (limited to 5, with geographical names)
INSERT INTO department (name) VALUES 
('Northern Division'),
('Southern Division'),
('Eastern Division'),
('Western Division'),
('Central Division');

-- Sites (10x more - 20 sites)
INSERT INTO site (department_id, address) VALUES
-- Northern Division sites
(1, '123 Build St, Construct City'),
(1, '234 North Ave, Highland Park'),
(1, '345 Arctic Rd, Northville'),
(1, '456 Frost Ln, Polarton'),
-- Southern Division sites
(2, '456 Project Ave, Construct City'),
(2, '567 Palm Blvd, Southport'),
(2, '678 Sunny St, Beachside'),
(2, '789 Ocean Dr, Coastville'),
-- Eastern Division sites
(3, '890 Sunrise Way, Eastborough'),
(3, '901 River St, Harborside'),
(3, '112 Mountain View, Clifftown'),
(3, '223 Valley Rd, Eastdale'),
-- Western Division sites
(4, '334 Sunset Blvd, Westchester'),
(4, '445 Desert Rd, Sandville'),
(4, '556 Canyon St, Westridge'),
(4, '667 Pacific Hwy, Oceanview'),
-- Central Division sites
(5, '778 Central Ave, Midtown'),
(5, '889 Downtown Rd, Centerburg'),
(5, '990 City Center, Metropolis'),
(5, '111 Main St, Centralville');

-- Customers (10x more - 20 customers)
INSERT INTO customer (name, address, phone, email) VALUES
('Urban Planning Co.', '789 Capital Blvd, Capital City', '+12345678901', 'contact@urbanplan.co'),
('HealthInfra Ltd.', '321 Health Dr, Medic Town', '+19876543210', 'info@healthinfra.com'),
('EduBuild Corp', '432 School Ln, Learnville', '+13334445555', 'projects@edubuild.org'),
('ResidentialTowers Inc', '543 Apartment St, Hometon', '+14445556666', 'info@restowers.com'),
('GreenBridge Developers', '654 Eco Ave, Naturetown', '+15556667777', 'build@greenbridge.net'),
('Highway Solutions', '765 Transport Rd, Roadcity', '+16667778888', 'contracts@highwaysolutions.com'),
('Industrial Park Ltd', '876 Factory St, Workersville', '+17778889999', 'development@indpark.biz'),
('Modern Living Spaces', '987 Design Ave, Styleville', '+18889990000', 'projects@modernliving.design'),
('City Infrastructure Group', '098 Municipal Rd, Govtown', '+19990001111', 'bids@cityinfra.gov'),
('Retail Developments Inc', '109 Shopping St, Mallville', '+10001112222', 'construction@retaildev.com'),
('Sports Complex Authority', '210 Stadium Rd, Athleticville', '+11112223333', 'facilities@sportscomplex.org'),
('Community Housing Trust', '321 Affordable Ln, Equitytown', '+12223334444', 'projects@communityhousing.org'),
('Tech Campus Builders', '432 Innovation Ave, Futureburgh', '+13334445555', 'development@techcampus.tech'),
('Tourism Resorts Limited', '543 Vacation Rd, Relaxville', '+14445556666', 'construction@tourismresorts.travel'),
('Airport Authority', '654 Terminal St, Flighttown', '+15556667777', 'development@airport.aero'),
('Maritime Facilities Corp', '765 Harbor Dr, Portcity', '+16667778888', 'projects@maritimefacilities.marine'),
('Healthcare Development Group', '876 Medical Blvd, Wellnessville', '+17778889999', 'construction@healthdev.care'),
('Energy Infrastructure Inc', '987 Power St, Gridtown', '+18889990000', 'projects@energyinfra.power'),
('AgriStorage Solutions', '098 Farm Rd, Harvestville', '+19990001111', 'facilities@agristorage.farm'),
('Cultural Center Foundation', '109 Museum Ave, Artsville', '+10001112222', 'development@culturalcenter.arts');

-- Grades (limited to 5)
INSERT INTO grade (grade, bonus_payment) VALUES 
('Trainee', 50.0),
('Junior', 100.0),
('Middle', 200.0),
('Senior', 300.0),
('Expert', 500.0);

-- Worker Types (10x more - 20 types)
INSERT INTO worker_type (type, description) VALUES
('Electrician', 'Handles electrical systems'),
('Plumber', 'Handles water and piping'),
('Carpenter', 'Works with wood structures'),
('Mason', 'Constructs with brick and stone'),
('Steel Worker', 'Handles steel structures'),
('Concrete Worker', 'Deals with concrete pouring and finishing'),
('Roofer', 'Installs and repairs roofs'),
('Painter', 'Applies paint and finishes'),
('Glazier', 'Installs glass fixtures'),
('HVAC Technician', 'Installs heating and cooling systems'),
('Insulation Installer', 'Insulates building structures'),
('Flooring Installer', 'Lays flooring materials'),
('Equipment Operator', 'Operates heavy construction equipment'),
('Scaffolder', 'Builds and dismantles scaffolding'),
('Laborer', 'General construction work'),
('Welder', 'Joins metal components'),
('Drywaller', 'Installs drywall panels'),
('Tiler', 'Lays ceramic and stone tiles'),
('Landscaper', 'Handles exterior grounds'),
('Demolition Worker', 'Safely demolishes structures');

-- Specialist Types (10x more - 20 types)
INSERT INTO specialist_type (type, description) VALUES
('Engineer', 'Designs and oversees construction'),
('Architect', 'Designs blueprints and structure'),
('Project Manager', 'Oversees project execution'),
('Structural Engineer', 'Designs structural elements'),
('Civil Engineer', 'Designs infrastructure components'),
('Mechanical Engineer', 'Designs mechanical systems'),
('Electrical Engineer', 'Designs electrical systems'),
('Environmental Engineer', 'Handles environmental impacts'),
('Safety Inspector', 'Ensures site safety compliance'),
('Quality Control', 'Monitors construction quality'),
('Quantity Surveyor', 'Estimates and tracks costs'),
('Interior Designer', 'Designs building interiors'),
('Urban Planner', 'Plans urban development'),
('Landscaping Architect', 'Designs outdoor spaces'),
('Building Services Engineer', 'Designs building systems'),
('Geotechnical Engineer', 'Analyzes soil conditions'),
('Sustainability Consultant', 'Advises on green building'),
('Traffic Engineer', 'Plans traffic management'),
('Acoustics Engineer', 'Designs sound environments'),
('Fire Safety Engineer', 'Designs fire protection systems');

-- Objects (10x more - 30 objects)
INSERT INTO object (name, type, site_id, customer_id, planned_start_date, planned_end_date, actual_start_date, actual_end_date) VALUES
-- Completed objects
('Central Bridge', 'bridge', 1, 1, '2023-01-01', '2023-12-31', '2023-01-15', '2023-12-20'),
('Riverfront Walkway', 'bridge', 3, 1, '2023-02-15', '2023-10-15', '2023-03-01', '2023-09-10'),
('Harbour Connection Bridge', 'bridge', 10, 16, '2023-03-10', '2023-11-30', '2023-03-20', '2023-11-15'),
('Mountain View High School', 'school', 11, 3, '2023-01-05', '2023-12-15', '2023-01-20', '2023-12-10'),
('Westside Elementary', 'school', 14, 3, '2023-04-10', '2023-11-20', '2023-04-15', '2023-11-10'),
('Oakwood Residences', 'residential', 6, 4, '2023-02-01', '2023-10-31', '2023-02-10', '2023-10-25'),
('Coastal Highway Section A', 'road', 8, 6, '2023-03-15', '2023-09-15', '2023-03-20', '2023-09-30'),
('Northern Expressway', 'road', 4, 6, '2023-05-01', '2023-11-30', '2023-05-15', '2023-12-05'),
('Community Medical Center', 'hospital', 19, 17, '2023-01-10', '2023-12-20', '2023-01-25', '2023-12-15'),
('Sports Arena', 'residential', 15, 11, '2023-02-15', '2023-11-15', '2023-03-01', '2023-11-30'),

-- In progress objects
('Sunrise Apartments', 'residential', 2, 1, '2024-03-01', '2025-03-01', '2024-03-05', NULL),
('Mediclinic Hospital', 'hospital', 1, 2, '2024-05-01', '2026-05-01', '2024-05-10', NULL),
('Downtown Skyscraper', 'residential', 18, 4, '2024-01-15', '2025-07-15', '2024-02-01', NULL),
('Tech Innovation Campus', 'school', 9, 13, '2024-02-01', '2025-06-01', '2024-02-15', NULL),
('Central Park Hospital', 'hospital', 20, 2, '2024-04-15', '2026-01-15', '2024-05-01', NULL),
('Eastside Mall', 'residential', 12, 10, '2024-03-01', '2025-09-01', '2024-03-15', NULL),
('Harbor Bridge', 'bridge', 11, 16, '2024-02-15', '2025-02-15', '2024-03-01', NULL),
('Airport Terminal Expansion', 'residential', 16, 15, '2024-01-10', '2025-04-10', '2024-01-20', NULL),
('Southern Highway Extension', 'road', 7, 6, '2024-03-01', '2025-01-01', '2024-03-10', NULL),
('North-South Connector', 'bridge', 4, 9, '2024-05-15', '2025-11-15', '2024-06-01', NULL),

-- Planned objects (not started yet)
('Green Energy Research Center', 'school', 5, 13, '2024-07-01', '2025-12-31', NULL, NULL),
('Coastal Health Clinic', 'hospital', 8, 17, '2024-08-15', '2025-10-15', NULL, NULL),
('Mountain Resort Complex', 'residential', 11, 14, '2024-09-01', '2026-03-01', NULL, NULL),
('Urban Renewal Housing', 'residential', 19, 12, '2024-07-15', '2025-08-15', NULL, NULL),
('Industrial Park Phase 1', 'residential', 13, 7, '2024-08-01', '2025-09-01', NULL, NULL),
('Agricultural Storage Facility', 'residential', 17, 19, '2024-09-15', '2025-06-15', NULL, NULL),
('Cultural Arts Center', 'school', 20, 20, '2024-10-01', '2026-04-01', NULL, NULL),
('Metropolitan Transit Hub', 'residential', 18, 9, '2024-11-15', '2026-11-15', NULL, NULL),
('Eastern Parkway', 'road', 12, 6, '2024-07-01', '2025-05-01', NULL, NULL),
('Western Scenic Byway', 'road', 16, 6, '2024-10-15', '2025-08-15', NULL, NULL);

-- Subtype objects - Bridge
INSERT INTO bridge_object (object_id, length_m, width_m) VALUES
(1, 120.5, 20.3),
(2, 85.3, 15.7),
(3, 150.8, 22.5),
(17, 200.3, 25.0),
(20, 175.6, 18.9);

-- Subtype objects - Residential
INSERT INTO residential_object (object_id, floors_number, has_parking_lot) VALUES
(2, 15, TRUE),
(6, 8, TRUE),
(10, 2, FALSE),
(11, 18, TRUE),
(13, 40, TRUE),
(16, 3, TRUE),
(18, 5, TRUE),
(23, 12, TRUE),
(24, 6, TRUE),
(25, 2, TRUE),
(26, 1, FALSE),
(28, 4, TRUE);

-- Subtype objects - Hospital
INSERT INTO hospital_object (object_id, beds_number) VALUES
(3, 300),
(9, 150),
(15, 450),
(22, 75);

-- Subtype objects - School
INSERT INTO school_object (object_id, classrooms_number) VALUES
(4, 40),
(5, 25),
(14, 60),
(21, 30),
(27, 45);

-- Subtype objects - Road
INSERT INTO road_object (object_id, length_m) VALUES
(7, 8500.0),
(8, 12300.0),
(19, 9700.0),
(29, 6800.0),
(30, 11200.0);

-- Work Types (10x more - 20 types)
INSERT INTO work_type (name, description) VALUES
('Foundation', 'Laying building foundations'),
('Framing', 'Constructing structural frames'),
('Excavation', 'Digging and earth moving'),
('Demolition', 'Controlled destruction of existing structures'),
('Electrical', 'Installing electrical systems'),
('Plumbing', 'Installing water and waste systems'),
('HVAC', 'Installing heating and cooling systems'),
('Roofing', 'Installing and waterproofing roofs'),
('Masonry', 'Brickwork and stonework'),
('Concrete Pouring', 'Pouring and finishing concrete'),
('Painting', 'Surface preparation and painting'),
('Flooring', 'Installing floor coverings'),
('Landscaping', 'Exterior grounds work'),
('Interior Finishing', 'Final interior detailing'),
('Glazing', 'Installing windows and glass'),
('Insulation', 'Installing thermal and acoustic insulation'),
('Steel Erection', 'Installing structural steel'),
('Asphalt Paving', 'Road surface construction'),
('Underground Utilities', 'Installing below-ground services'),
('Final Inspection', 'Quality control and approval process');

-- Materials (10x more - 20 materials)
INSERT INTO material (name, description, unit, price) VALUES
('Concrete', 'High strength', 'kg', 100.0),
('Rebar', 'Steel reinforcement', 'pieces', 50.0),
('Lumber', 'Construction grade wood', 'boards', 75.0),
('Bricks', 'Standard red clay', 'pieces', 2.0),
('Steel Beams', 'Structural I-beams', 'pieces', 500.0),
('Glass Panels', 'Tempered safety glass', 'panels', 300.0),
('Copper Wiring', 'Electrical grade', 'meters', 30.0),
('PVC Pipes', 'For plumbing', 'meters', 25.0),
('Roof Tiles', 'Ceramic weather resistant', 'pieces', 10.0),
('Insulation Material', 'Fiberglass rolls', 'rolls', 80.0),
('Paint', 'Interior latex', 'liters', 40.0),
('Drywall', 'Standard gypsum', 'sheets', 35.0),
('Cement', 'Portland type', 'bags', 20.0),
('Mortar', 'For masonry work', 'bags', 15.0),
('Gravel', 'Mixed sizes', 'tons', 150.0),
('Sand', 'Fine grain', 'tons', 120.0),
('Asphalt', 'Road grade', 'tons', 200.0),
('Ceiling Tiles', 'Acoustic panels', 'pieces', 25.0),
('HVAC Ducts', 'Galvanized steel', 'meters', 45.0),
('Floor Tiles', 'Ceramic', 'pieces', 8.0);

-- Machine Types (10x more - 20 types)
INSERT INTO machine_type (type, description) VALUES
('Excavator', 'Heavy digging machine'),
('Crane', 'Lifting machine'),
('Bulldozer', 'Earth moving machine'),
('Loader', 'Front-loading material handler'),
('Grader', 'Surface leveling machine'),
('Paver', 'Asphalt laying machine'),
('Concrete Mixer', 'Mixes concrete on site'),
('Roller', 'Compacting machine'),
('Dump Truck', 'Material hauling vehicle'),
('Forklift', 'Materials lifting and moving'),
('Backhoe', 'Digging and loading machine'),
('Trencher', 'Digging trenches for pipes and cables'),
('Pile Driver', 'Drives foundation piles into ground'),
('Concrete Pump', 'Pumps liquid concrete to location'),
('Generator', 'Provides electrical power'),
('Air Compressor', 'Provides compressed air for tools'),
('Scissor Lift', 'Elevating work platform'),
('Cherry Picker', 'Elevated work platform with reach'),
('Drilling Rig', 'For ground testing and foundation work'),
('Skid Steer', 'Compact and versatile loader');

-- Machine Models (10x more - 20 models)
INSERT INTO machine_model (model, machine_type_id) VALUES
('CAT320', 1),
('Liebherr LTM', 2),
('Komatsu D65', 3),
('CAT 950', 4),
('John Deere 670G', 5),
('Volvo P6820C', 6),
('SANY SY306C-8', 1),
('Terex RT90', 2),
('Hitachi ZX350', 1),
('CAT 140M3', 5),
('Volvo L120', 4),
('Bobcat T650', 20),
('Wirtgen W200i', 6),
('Bomag BW213', 8),
('JCB 3CX', 11),
('Ditch Witch RT45', 12),
('Junttan PM20', 13),
('Putzmeister BSF36', 14),
('Atlas Copco XATS', 16),
('Genie GS-1930', 17);

-- Machines (10x more - 20 machines)
INSERT INTO machine (machine_model_id, department_id, production_year) VALUES
(1, 1, 2020),
(2, 2, 2019),
(3, 3, 2021),
(4, 4, 2020),
(5, 5, 2022),
(6, 1, 2021),
(7, 2, 2020),
(8, 3, 2019),
(9, 4, 2022),
(10, 5, 2021),
(11, 1, 2022),
(12, 2, 2021),
(13, 3, 2020),
(14, 4, 2019),
(15, 5, 2022),
(16, 1, 2020),
(17, 2, 2021),
(18, 3, 2022),
(19, 4, 2020),
(20, 5, 2021);

-- Create 60 employees (40 workers and 20 specialists)
-- Workers (40)
INSERT INTO employee (name, surname, middlename, phone, email, salary) VALUES
-- Current workers from initial script
('John', 'Doe', 'E', '+12223334444', 'john.doe@buildnova.com', 3000),
('Carol', 'White', 'D', '+15556667777', 'carol.white@buildnova.com', 3500),
-- New workers
('Michael', 'Johnson', 'R', '+12223334445', 'michael.johnson@buildnova.com', 3200),
('Thomas', 'Williams', 'H', '+12223334446', 'thomas.williams@buildnova.com', 3100),
('David', 'Jones', 'P', '+12223334447', 'david.jones@buildnova.com', 3400),
('Christopher', 'Brown', 'G', '+12223334448', 'christopher.brown@buildnova.com', 3600),
('Daniel', 'Davis', 'T', '+12223334449', 'daniel.davis@buildnova.com', 3300),
('Matthew', 'Miller', 'J', '+12223334450', 'matthew.miller@buildnova.com', 3700),
('Andrew', 'Wilson', 'K', '+12223334451', 'andrew.wilson@buildnova.com', 3000),
('Richard', 'Moore', 'L', '+12223334452', 'richard.moore@buildnova.com', 3200),
('Joshua', 'Taylor', 'M', '+12223334453', 'joshua.taylor@buildnova.com', 3400),
('James', 'Anderson', 'N', '+12223334454', 'james.anderson@buildnova.com', 3100),
('Ryan', 'Thomas', 'O', '+12223334455', 'ryan.thomas@buildnova.com', 3300),
('Justin', 'Jackson', 'P', '+12223334456', 'justin.jackson@buildnova.com', 3500),
('Brandon', 'White', 'Q', '+12223334457', 'brandon.white@buildnova.com', 3200),
('William', 'Harris', 'S', '+12223334458', 'william.harris@buildnova.com', 3100),
('Jonathan', 'Martin', 'U', '+12223334459', 'jonathan.martin@buildnova.com', 3400),
('Brian', 'Thompson', 'V', '+12223334460', 'brian.thompson@buildnova.com', 3600),
('Joseph', 'Garcia', 'W', '+12223334461', 'joseph.garcia@buildnova.com', 3300),
('Nicholas', 'Martinez', 'X', '+12223334462', 'nicholas.martinez@buildnova.com', 3700),
('Anthony', 'Robinson', 'Y', '+12223334463', 'anthony.robinson@buildnova.com', 3000),
('Robert', 'Clark', 'Z', '+12223334464', 'robert.clark@buildnova.com', 3200),
('Kevin', 'Rodriguez', 'A', '+12223334465', 'kevin.rodriguez@buildnova.com', 3400),
('Eric', 'Lewis', 'B', '+12223334466', 'eric.lewis@buildnova.com', 3100),
('Steven', 'Lee', 'C', '+12223334467', 'steven.lee@buildnova.com', 3300),
('Timothy', 'Walker', 'F', '+12223334468', 'timothy.walker@buildnova.com', 3500),
('Jason', 'Hall', 'I', '+12223334469', 'jason.hall@buildnova.com', 3200),
('Jeffrey', 'Allen', 'J', '+12223334470', 'jeffrey.allen@buildnova.com', 3100),
('Scott', 'Young', 'K', '+12223334471', 'scott.young@buildnova.com', 3400),
('Brandon', 'Hernandez', 'L', '+12223334472', 'brandon.hernandez@buildnova.com', 3600),
('Mark', 'King', 'M', '+12223334473', 'mark.king@buildnova.com', 3300),
('Jose', 'Wright', 'N', '+12223334474', 'jose.wright@buildnova.com', 3700),
('Kenneth', 'Lopez', 'O', '+12223334475', 'kenneth.lopez@buildnova.com', 3000),
('Gregory', 'Hill', 'P', '+12223334476', 'gregory.hill@buildnova.com', 3200),
('Ronald', 'Scott', 'Q', '+12223334477', 'ronald.scott@buildnova.com', 3400),
('Larry', 'Green', 'R', '+12223334478', 'larry.green@buildnova.com', 3100),
('Frank', 'Adams', 'S', '+12223334479', 'frank.adams@buildnova.com', 3300),
('Patrick', 'Baker', 'T', '+12223334480', 'patrick.baker@buildnova.com', 3500),
('Gary', 'Gonzalez', 'U', '+12223334481', 'gary.gonzalez@buildnova.com', 3200),
('Raymond', 'Nelson', 'V', '+12223334482', 'raymond.nelson@buildnova.com', 3100),
('Dennis', 'Carter', 'W', '+12223334483', 'dennis.carter@buildnova.com', 3400);

-- Specialists (20)
INSERT INTO employee (name, surname, middlename, phone, email, salary) VALUES
-- Current specialists from initial script
('Alice', 'Smith', 'B', '+13334445555', 'alice.smith@buildnova.com', 4000),
('Bob', 'Brown', 'C', '+14445556666', 'bob.brown@buildnova.com', 5000),
-- New specialists
('Sarah', 'Johnson', 'A', '+13334445556', 'sarah.johnson@buildnova.com', 4200),
('Jennifer', 'Williams', 'B', '+13334445557', 'jennifer.williams@buildnova.com', 4500),
('Elizabeth', 'Jones', 'C', '+13334445558', 'elizabeth.jones@buildnova.com', 4800),
('Jessica', 'Brown', 'D', '+13334445559', 'jessica.brown@buildnova.com', 5200),
('Lisa', 'Davis', 'E', '+13334445560', 'lisa.davis@buildnova.com', 4300),
('Sandra', 'Miller', 'F', '+13334445561', 'sandra.miller@buildnova.com', 4600),
('Helen', 'Wilson', 'G', '+13334445562', 'helen.wilson@buildnova.com', 4900),
('Ashley', 'Moore', 'H', '+13334445563', 'ashley.moore@buildnova.com', 5300),
('Donna', 'Taylor', 'I', '+13334445564', 'donna.taylor@buildnova.com', 4400),
('Kimberly', 'Anderson', 'J', '+13334445565', 'kimberly.anderson@buildnova.com', 4700),
('Michelle', 'Thomas', 'K', '+13334445566', 'michelle.thomas@buildnova.com', 5000),
('Emily', 'Jackson', 'L', '+13334445567', 'emily.jackson@buildnova.com', 5400),
('Amanda', 'White', 'M', '+13334445568', 'amanda.white@buildnova.com', 4500),
('Stephanie', 'Harris', 'N', '+13334445569', 'stephanie.harris@buildnova.com', 4800),
('Rebecca', 'Martin', 'O', '+13334445570', 'rebecca.martin@buildnova.com', 5100),
('Laura', 'Thompson', 'P', '+13334445571', 'laura.thompson@buildnova.com', 4600),
('Sharon', 'Garcia', 'Q', '+13334445572', 'sharon.garcia@buildnova.com', 4900),
('Cynthia', 'Martinez', 'R', '+13334445573', 'cynthia.martinez@buildnova.com', 5200);

-- Assign worker types
INSERT INTO worker (employee_id, worker_type_id) VALUES
(1, 1), -- John Doe - Electrician
(2, 2), -- Carol White - Plumber
(3, 3), -- Michael Johnson - Carpenter
(4, 4), -- Thomas Williams - Mason
(5, 5), -- David Jones - Steel Worker
(6, 6), -- Christopher Brown - Concrete Worker
(7, 7), -- Daniel Davis - Roofer
(8, 8), -- Matthew Miller - Painter
(9, 9), -- Andrew Wilson - Glazier
(10, 10), -- Richard Moore - HVAC Technician
(11, 11), -- Joshua Taylor - Insulation Installer
(12, 12), -- James Anderson - Flooring Installer
(13, 13), -- Ryan Thomas - Equipment Operator
(14, 14), -- Justin Jackson - Scaffolder
(15, 15), -- Brandon White - Laborer
(16, 16), -- William Harris - Welder
(17, 17), -- Jonathan Martin - Drywaller
(18, 18), -- Brian Thompson - Tiler
(19, 19), -- Joseph Garcia - Landscaper
(20, 20), -- Nicholas Martinez - Demolition Worker
(21, 1), -- Anthony Robinson - Electrician
(22, 2), -- Robert Clark - Plumber
(23, 3), -- Kevin Rodriguez - Carpenter
(24, 4), -- Eric Lewis - Mason
(25, 5), -- Steven Lee - Steel Worker
(26, 6), -- Timothy Walker - Concrete Worker
(27, 7), -- Jason Hall - Roofer
(28, 8), -- Jeffrey Allen - Painter
(29, 9), -- Scott Young - Glazier
(30, 10), -- Brandon Hernandez - HVAC Technician
(31, 11), -- Mark King - Insulation Installer
(32, 12), -- Jose Wright - Flooring Installer
(33, 13), -- Kenneth Lopez - Equipment Operator
(34, 14), -- Gregory Hill - Scaffolder
(35, 15), -- Ronald Scott - Laborer
(36, 16), -- Larry Green - Welder
(37, 17), -- Frank Adams - Drywaller
(38, 18), -- Patrick Baker - Tiler
(39, 19), -- Gary Gonzalez - Landscaper
(40, 20); -- Raymond Nelson - Demolition Worker

-- Assign specialist types
INSERT INTO specialist (employee_id, specialist_type_id) VALUES
(41, 1), -- Alice Smith - Engineer
(42, 2), -- Bob Brown - Architect
(43, 3), -- Sarah Johnson - Project Manager
(44, 4), -- Jennifer Williams - Structural Engineer
(45, 5), -- Elizabeth Jones - Civil Engineer
(46, 6), -- Jessica Brown - Mechanical Engineer
(47, 7), -- Lisa Davis - Electrical Engineer
(48, 8), -- Sandra Miller - Environmental Engineer
(49, 9), -- Helen Wilson - Safety Inspector
(50, 10), -- Ashley Moore - Quality Control
(51, 11), -- Donna Taylor - Quantity Surveyor
(52, 12), -- Kimberly Anderson - Interior Designer
(53, 13), -- Michelle Thomas - Urban Planner
(54, 14), -- Emily Jackson - Landscaping Architect
(55, 15), -- Amanda White - Building Services Engineer
(56, 16), -- Stephanie Harris - Geotechnical Engineer
(57, 17), -- Rebecca Martin - Sustainability Consultant
(58, 18), -- Laura Thompson - Traffic Engineer
(59, 19), -- Sharon Garcia - Acoustics Engineer
(60, 20); -- Cynthia Martinez - Fire Safety Engineer

-- Brigades (10 brigades)
INSERT INTO brigade (name) VALUES
('Alpha Team'),
('Beta Team'),
('Gamma Team'),
('Delta Team'),
('Epsilon Team'),
('Zeta Team'),
('Eta Team'),
('Theta Team'),
('Iota Team'),
('Kappa Team');

-- Brigade Workers history - showing movement between brigades over time
-- Alpha Team
INSERT INTO brigade_worker (brigade_id, worker_id, start_date, end_date) VALUES
(1, 1, '2023-01-01', '2023-06-30'), -- John Doe
(1, 3, '2023-01-01', '2023-12-31'), -- Michael Johnson
(1, 5, '2023-01-01', NULL), -- David Jones
(1, 7, '2023-07-01', NULL), -- Daniel Davis
(1, 1, '2024-01-01', NULL); -- John Doe returns

-- Beta Team
INSERT INTO brigade_worker (brigade_id, worker_id, start_date, end_date) VALUES
(2, 9, '2023-01-01', '2023-09-30'), -- Andrew Wilson
(2, 11, '2023-01-01', NULL), -- Joshua Taylor
(2, 13, '2023-01-01', NULL), -- Ryan Thomas
(2, 15, '2023-10-01', NULL), -- Brandon White
(2, 9, '2024-02-01', NULL); -- Andrew Wilson returns

-- Gamma Team
INSERT INTO brigade_worker (brigade_id, worker_id, start_date, end_date) VALUES
(3, 17, '2023-01-01', '2023-12-31'), -- Jonathan Martin
(3, 19, '2023-01-01', '2023-08-31'), -- Joseph Garcia
(3, 21, '2023-01-01', NULL), -- Anthony Robinson
(3, 23, '2023-09-01', NULL), -- Kevin Rodriguez
(3, 19, '2024-01-01', NULL); -- Joseph Garcia returns

-- Delta Team
INSERT INTO brigade_worker (brigade_id, worker_id, start_date, end_date) VALUES
(4, 27, '2023-01-01', '2023-10-31'), -- Jason Hall
(4, 31, '2023-07-01', NULL), -- Mark King
(4, 27, '2024-01-01', NULL); -- Jason Hall returns

-- Epsilon Team
INSERT INTO brigade_worker (brigade_id, worker_id, start_date, end_date) VALUES
(5, 33, '2023-01-01', '2023-12-31'), -- Kenneth Lopez
(5, 35, '2023-01-01', NULL), -- Ronald Scott
(5, 39, '2024-01-01', NULL); -- Gary Gonzalez

-- Zeta Team
INSERT INTO brigade_worker (brigade_id, worker_id, start_date, end_date) VALUES
(6, 4, '2023-01-01', '2023-10-31'), -- Thomas Williams
(6, 6, '2023-01-01', NULL), -- Christopher Brown
(6, 8, '2023-10-01', NULL); -- Matthew Miller

-- Eta Team
INSERT INTO brigade_worker (brigade_id, worker_id, start_date, end_date) VALUES
(7, 10, '2023-01-01', '2023-09-30'), -- Richard Moore
(7, 12, '2023-01-01', NULL), -- James Anderson
(7, 16, '2023-10-01', '2023-11-01'); -- William Harris

-- Theta Team
INSERT INTO brigade_worker (brigade_id, worker_id, start_date, end_date) VALUES
(8, 18, '2023-01-01', '2023-12-31'), -- Brian Thompson
(8, 22, '2023-01-01', '2023-09-13'), -- Robert Clark
(8, 24, '2023-09-01', NULL), -- Eric Lewis
(8, 18, '2024-01-15', NULL); -- Brian Thompson returns

-- Iota Team
INSERT INTO brigade_worker (brigade_id, worker_id, start_date, end_date) VALUES
(9, 28, '2023-01-01', '2023-10-31'), -- Jeffrey Allen
(9, 30, '2023-01-01', NULL), -- Brandon Hernandez
(9, 26, '2024-01-01', NULL); -- Timothy Walker returns

-- Kappa Team
INSERT INTO brigade_worker (brigade_id, worker_id, start_date, end_date) VALUES
(10, 34, '2023-01-01', '2023-12-31'), -- Gregory Hill
(10, 36, '2023-01-01', '2023-12-31'), -- Larry Green
(10, 40, '2024-01-01', NULL), -- Raymond Nelson
(10, 34, '2024-03-01', NULL); -- Gregory Hill returns

-- Brigadiers history
INSERT INTO brigadier (brigade_id, worker_id, start_date, end_date) VALUES
(1, 2, '2023-01-01', '2023-12-31'), -- Carol White
(1, 4, '2024-01-01', NULL), -- Thomas Williams
(2, 8, '2023-01-01', '2023-09-30'), -- Matthew Miller
(2, 10, '2023-10-01', NULL), -- Richard Moore
(3, 14, '2023-01-01', '2023-12-31'), -- Justin Jackson
(3, 16, '2024-01-01', NULL), -- William Harris
(4, 20, '2023-01-01', '2023-12-31'), -- Nicholas Martinez
(4, 22, '2024-01-01', NULL), -- Robert Clark
(5, 26, '2023-01-01', '2023-12-31'), -- Timothy Walker
(5, 28, '2024-01-01', NULL), -- Jeffrey Allen
(6, 32, '2023-01-01', '2023-12-31'), -- Jose Wright
(6, 34, '2024-01-01', '2024-02-28'), -- Gregory Hill
(6, 36, '2024-03-01', NULL), -- Larry Green
(7, 38, '2023-01-01', NULL), -- Patrick Baker
(8, 25, '2023-01-01', NULL), -- Steven Lee
(9, 29, '2023-01-01', '2023-12-31'), -- Scott Young
(9, 33, '2024-01-01', NULL), -- Kenneth Lopez
(10, 37, '2023-01-01', NULL); -- Frank Adams

-- Site Specialists assignments
-- Northern Division
INSERT INTO site_specialist (site_id, specialist_id, start_date, end_date) VALUES
(1, 41, '2023-01-01', '2023-12-31'), -- Alice Smith at site 1
(1, 43, '2024-01-01', NULL); -- Sarah Johnson at site 1

-- Southern Division
INSERT INTO site_specialist (site_id, specialist_id, start_date, end_date) VALUES
(5, 53, '2023-01-01', '2023-12-31'), -- Michelle Thomas at site 5
(5, 55, '2024-01-01', NULL), -- Amanda White at site 5
(6, 57, '2023-01-01', '2023-09-30'), -- Rebecca Martin at site 6
(7, 42, '2022-01-01', '2022-09-01'), -- Bob Brown at site 7
(8, 44, '2022-01-01', '2022-10-01'); -- Jennifer Williams at site 8

-- Eastern Division
INSERT INTO site_specialist (site_id, specialist_id, start_date, end_date) VALUES
(9, 46, '2023-01-01', '2023-12-31'), -- Jessica Brown at site 9
(9, 48, '2024-01-01', NULL), -- Sandra Miller at site 9
(10, 50, '2023-01-01', '2023-09-30'), -- Ashley Moore at site 10
(10, 52, '2022-10-01', '2022-11-03'), -- Kimberly Anderson at site 10
(11, 54, '2023-01-01', '2023-08-02'), -- Emily Jackson at site 11
(12, 56, '2022-05-05', '2022-08-08'); -- Stephanie Harris at site 12

-- Western Division
INSERT INTO site_specialist (site_id, specialist_id, start_date, end_date) VALUES
(13, 58, '2023-01-01', '2023-12-31'), -- Laura Thompson at site 13
(13, 60, '2024-01-01', NULL), -- Cynthia Martinez at site 13
(14, 43, '2023-01-01', '2023-09-30'), -- Sarah Johnson at site 14
(14, 45, '2023-10-01', NULL), -- Elizabeth Jones at site 14
(15, 47, '2023-01-01', NULL), -- Lisa Davis at site 15
(16, 49, '2023-01-01', NULL); -- Helen Wilson at site 16

-- Central Division
INSERT INTO site_specialist (site_id, specialist_id, start_date, end_date) VALUES
(17, 53, '2024-01-01', NULL), -- Michelle Thomas at site 17
(18, 55, '2023-01-01', '2023-09-30'), -- Amanda White at site 18
(18, 57, '2023-10-01', NULL), -- Rebecca Martin at site 18
(19, 59, '2023-01-01', NULL), -- Sharon Garcia at site 19
(20, 42, '2023-01-01', NULL); -- Bob Brown at site 20

-- Site Supervisors
INSERT INTO site_supervisor (site_id, specialist_id, start_date, end_date) VALUES
(1, 44, '2023-01-01', '2023-12-31'), -- Jennifer Williams at site 1
(1, 46, '2024-01-01', NULL), -- Jessica Brown at site 1
(5, 48, '2023-01-01', '2023-12-31'), -- Sandra Miller at site 5
(5, 50, '2024-01-01', NULL), -- Ashley Moore at site 5
(9, 52, '2023-01-01', '2023-12-31'), -- Kimberly Anderson at site 9
(9, 54, '2024-01-01', NULL), -- Emily Jackson at site 9
(13, 56, '2023-01-01', '2023-12-31'), -- Stephanie Harris at site 13
(13, 58, '2024-01-01', NULL), -- Laura Thompson at site 13
(17, 60, '2023-01-01', '2023-12-31'), -- Cynthia Martinez at site 17
(17, 41, '2024-01-01', NULL); -- Alice Smith at site 17

-- Department Supervisors
INSERT INTO department_supervisor (department_id, specialist_id, start_date, end_date) VALUES
(1, 43, '2023-01-01', '2023-12-31'), -- Sarah Johnson at Northern Division
(1, 45, '2024-01-01', NULL), -- Elizabeth Jones at Northern Division
(2, 47, '2023-01-01', '2023-12-31'), -- Lisa Davis at Southern Division
(2, 49, '2024-01-01', NULL), -- Helen Wilson at Southern Division
(3, 51, '2023-01-01', '2023-12-31'), -- Donna Taylor at Eastern Division
(3, 53, '2024-01-01', NULL), -- Michelle Thomas at Eastern Division
(4, 55, '2023-01-01', '2023-12-31'), -- Amanda White at Western Division
(4, 57, '2024-01-01', NULL), -- Rebecca Martin at Western Division
(5, 59, '2023-01-01', '2023-12-31'), -- Sharon Garcia at Central Division
(5, 42, '2024-01-01', NULL); -- Bob Brown at Central Division

-- Work schedules for objects
-- For completed objects
INSERT INTO work_schedule (object_id, work_type_id, brigade_id, planned_start_date, planned_end_date, actual_start_date, actual_end_date) VALUES
-- Central Bridge (object_id 1)
(1, 1, 1, '2023-01-15', '2023-02-28', '2023-01-15', '2023-02-25'),  -- Foundation
(1, 3, 2, '2023-03-01', '2023-04-15', '2023-03-05', '2023-04-10'),  -- Excavation
(1, 17, 3, '2023-04-20', '2023-07-30', '2023-04-25', '2023-08-05'), -- Steel Erection
(1, 10, 4, '2023-08-10', '2023-10-15', '2023-08-15', '2023-10-20'), -- Concrete Pouring
(1, 20, 5, '2023-11-01', '2023-12-15', '2023-11-05', '2023-12-20'), -- Final Inspection

-- Riverfront Walkway (object_id 2)
(2, 3, 6, '2023-03-01', '2023-03-30', '2023-03-01', '2023-03-25'),  -- Excavation
(2, 1, 7, '2023-04-01', '2023-04-30', '2023-04-05', '2023-05-05'),  -- Foundation
(2, 10, 8, '2023-05-10', '2023-06-15', '2023-05-15', '2023-06-20'), -- Concrete Pouring
(2, 13, 9, '2023-06-20', '2023-07-30', '2023-06-25', '2023-08-05'), -- Landscaping
(2, 20, 10, '2023-08-10', '2023-09-10', '2023-08-15', '2023-09-10'), -- Final Inspection

-- In-progress objects
-- Sunrise Apartments (object_id 11)
(11, 3, 1, '2024-03-05', '2024-04-05', '2024-03-05', '2024-04-10'), -- Excavation
(11, 1, 2, '2024-04-15', '2024-05-30', '2024-04-20', NULL),         -- Foundation
(11, 2, 3, '2024-06-01', '2024-08-15', NULL, NULL),                 -- Framing
(11, 5, 4, '2024-08-20', '2024-10-30', NULL, NULL),                 -- Electrical
(11, 6, 5, '2024-11-05', '2025-01-15', NULL, NULL),                 -- Plumbing

-- Mediclinic Hospital (object_id 12)
(12, 4, 6, '2024-05-10', '2024-06-30', '2024-05-10', '2024-07-05'), -- Demolition
(12, 3, 7, '2024-07-10', '2024-08-30', '2024-07-15', NULL),         -- Excavation
(12, 1, 8, '2024-09-05', '2024-11-15', NULL, NULL),                 -- Foundation
(12, 2, 9, '2024-11-20', '2025-02-28', NULL, NULL),                 -- Framing
(12, 5, 10, '2025-03-05', '2025-05-30', NULL, NULL);                -- Electrical

-- Material supplies for work schedules
-- For Central Bridge
INSERT INTO material_supply (material_id, work_schedule_id, planned_quantity, actual_quantity) VALUES
(1, 1, 5000, 5200),    -- Concrete for foundation
(2, 1, 2500, 2300),    -- Rebar for foundation
(15, 2, 1000, 950),    -- Gravel for excavation
(16, 2, 800, 820),     -- Sand for excavation
(5, 3, 300, 310),      -- Steel Beams for steel erection
(1, 4, 8000, 8100),    -- Concrete for concrete pouring
(2, 4, 4000, 3900);    -- Rebar for concrete pouring

-- For Riverfront Walkway
INSERT INTO material_supply (material_id, work_schedule_id, planned_quantity, actual_quantity) VALUES
(15, 6, 800, 780),     -- Gravel for excavation
(16, 6, 600, 590),     -- Sand for excavation
(1, 7, 3000, 3050),    -- Concrete for foundation
(2, 7, 1500, 1450),    -- Rebar for foundation
(1, 8, 5000, 5100),    -- Concrete for concrete pouring
(13, 8, 500, 520);     -- Cement for concrete pouring

-- For Sunrise Apartments
INSERT INTO material_supply (material_id, work_schedule_id, planned_quantity, actual_quantity) VALUES
(15, 11, 1200, 1180),  -- Gravel for excavation
(16, 11, 900, 910),    -- Sand for excavation
(1, 12, 6000, 5800),   -- Concrete for foundation
(2, 12, 3000, 2950),   -- Rebar for foundation
(3, 13, 2000, 0),      -- Lumber for framing (not delivered yet)
(5, 13, 500, 0),       -- Steel Beams for framing (not delivered yet)
(7, 14, 5000, 0),      -- Copper Wiring for electrical (not delivered yet)
(8, 15, 3000, 0);      -- PVC Pipes for plumbing (not delivered yet)

-- For Mediclinic Hospital
INSERT INTO material_supply (material_id, work_schedule_id, planned_quantity, actual_quantity) VALUES
(15, 16, 2000, 2100),  -- Gravel for demolition
(15, 17, 3000, 2900),  -- Gravel for excavation
(16, 17, 2500, 2450),  -- Sand for excavation
(1, 18, 10000, 0),     -- Concrete for foundation (not delivered yet)
(2, 18, 5000, 0),      -- Rebar for foundation (not delivered yet)
(3, 19, 3000, 0),      -- Lumber for framing (not delivered yet)
(5, 19, 1000, 0),      -- Steel Beams for framing (not delivered yet)
(7, 20, 8000, 0);      -- Copper Wiring for electrical (not delivered yet)

-- Site machine assignments
INSERT INTO site_machine (site_id, machine_id, start_date, end_date) VALUES
-- Historical assignments
(1, 1, '2023-01-10', '2023-12-20'),  -- Excavator at Central Bridge site
(2, 2, '2023-02-15', '2023-09-10'),  -- Crane at Riverfront Walkway site
(3, 3, '2023-03-01', '2023-08-15'),  -- Bulldozer at Site 3
(4, 4, '2023-04-10', '2023-10-30'),  -- Loader at Site 4
(5, 5, '2023-05-15', '2023-11-15'),  -- Grader at Site 5
-- Current assignments
(2, 1, '2024-01-10', NULL),          -- Excavator moved to Site 2
(11, 2, '2024-03-01', NULL),         -- Crane moved to Sunrise Apartments site
(12, 3, '2024-05-05', NULL),         -- Bulldozer moved to Mediclinic Hospital site
(9, 4, '2024-01-15', NULL),          -- Loader moved to Site 9
(14, 5, '2024-02-01', NULL),         -- Grader moved to Site 14
(6, 6, '2023-06-01', '2023-12-15'),  -- Machine 6 at Site 6
(7, 7, '2023-07-10', '2024-01-20'),  -- Machine 7 at Site 7
(8, 8, '2023-08-15', '2024-02-10'),  -- Machine 8 at Site 8
(13, 9, '2023-09-01', '2024-03-15'), -- Machine 9 at Site 13
(18, 10, '2023-10-10', NULL),        -- Machine 10 at Site 18
(19, 11, '2023-11-15', NULL),        -- Machine 11 at Site 19
(20, 12, '2023-12-01', NULL),        -- Machine 12 at Site 20
(5, 13, '2024-01-05', NULL),         -- Machine 13 at Site 5
(7, 14, '2024-02-15', NULL),         -- Machine 14 at Site 7
(8, 15, '2024-03-10', NULL),         -- Machine 15 at Site 8
(10, 16, '2024-04-01', NULL),        -- Machine 16 at Site 10
(15, 17, '2024-05-05', NULL),        -- Machine 17 at Site 15
(16, 18, '2024-06-10', NULL),        -- Machine 18 at Site 16
(17, 19, '2023-12-15', NULL),        -- Machine 19 at Site 17
(3, 20, '2024-02-20', NULL);         -- Machine 20 at Site 3
