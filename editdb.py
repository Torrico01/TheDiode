import sqlite3
db = sqlite3.connect("db.sqlite3")

cur = db.cursor()

#cur.execute("UPDATE django_content_type SET app_label='projects' WHERE app_label='projetos'")
#cur.execute("ALTER TABLE projects_modularstoragepannel RENAME TO projects_modularstoragepanel")
#cur.execute("UPDATE components_category SET app='name' WHERE app='nome'")
#cur.execute("REPLACE INTO django_content_type ('id','app_label','model') VALUES('19','components','components5')")
#cur.execute("DELETE FROM django_content_type WHERE ID = 19")
cur.execute("ALTER TABLE components_component RENAME COLUMN 'nome' TO 'name'")
db.commit()
cur.close()
db.close()