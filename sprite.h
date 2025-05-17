#ifndef SPRITE_H
#define SPRITE_H

#include <QGraphicsWidget>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QPainter>
#include <QList>
#include <QGraphicsSceneMouseEvent>
#include <QJsonObject>
#include <QJsonArray>
#include <QColor>

#include <QSettings>

enum BUTTONS {TRANSPARENT, COLOR, MC1, MC2, OVERLAY_COLOR, OVERLAY_TRANSPARENT };
enum PEN { DEFAULT, FLOOD };

class SpriteView;
class Sprite;
struct options {
    int left_button = BUTTONS::COLOR;
    int right_button = BUTTONS::TRANSPARENT;
    int pen = PEN::DEFAULT;
    QJsonObject data;
    int selection_from = 0;
    int selection_to = 0;
    QList<Sprite*> sprite_list;
    SpriteView *spriteview;
    bool show_grid_lines = true;
    bool show_numbers = false;


    //auto export
    QString last_exported_file;
    int export_address = 0x3000;
    int export_file_format;
    int export_attribute_format;
    //auto save
    QString last_saved_file;

    //hashquestion
    QByteArray last_hash;

    //
    QList<QJsonObject> undoDB;

    //QColor background = QColor(0xd9,0xd6,0xc8);
    //QColor selection_color = QColor(0x00,0xff,0x00);
    //int sprite_spacing_x = 30;
    //int sprite_spacing_y = 30;
    //int sprites_per_row = 4;


    QStringList col_names = {"Black","White","Red","Cyan","Purple","Green","Blue","Yellow","Orange","Brown",
                             "Pink", "Dark Grey", "Grey","Light Green","Light Blue","Light Grey"};
    QList<QColor> col_list = { QColor(0,0,0),
                               QColor(255,255,255),
                               QColor(0x92,0x4a,0x40),
                               QColor(0x84,0xc5,0xcc),
                               QColor(0x93,0x51,0xb6),
                               QColor(0x72,0xb1,0x4b),
                               QColor(0x48,0x3a,0xaa),
                               QColor(0xd5,0xdf,0x7c),
                               QColor(0x99,0x69,0x2d),
                               QColor(0x67,0x52,0x00),
                               QColor(0xc1,0x81,0x7a),
                               QColor(0x60,0x60,0x60),
                               QColor(0x8a,0x8a,0x8a),
                               QColor(0xb3,0xec,0x91),
                               QColor(0x86,0x7a,0xde),
                               QColor(0xb3,0xb3,0xb3)};
};

class Sprite : public QGraphicsItem
{
public:
    Sprite(options *opt, int id);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
    QRectF boundingRect() const override;



    void change_tile(QPointF pos);

    void mousePressEvent(QGraphicsSceneMouseEvent *ev) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *ev) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *ev) override;

    bool get_bit(int x, int y);
    void set_bit(int x, int y, bool value);

    void slide_up(){
        bool tmp[24];

        for (int x = 0; x < 24; x++)
            tmp[x] = this->get_bit(x,0);
        for (int y = 0; y < 20; y++)
        {
            for (int x = 0; x < 24; x++)
            {
                this->set_bit(x,y, this->get_bit(x,y+1));
            }
        }
        for (int x = 0; x < 24; x++)
            this->set_bit(x,20, tmp[x]);
    }
    void slide_down(){
        bool tmp[24];

        for (int x = 0; x < 24; x++)
            tmp[x] = this->get_bit(x,20);
        for (int y = 20; y > 0; y--)
        {
            for (int x = 0; x < 24; x++)
            {
                this->set_bit(x,y, this->get_bit(x,y-1));
            }
        }
        for (int x = 0; x < 24; x++)
            this->set_bit(x,0, tmp[x]);
    }
    void slide_left(){
        bool tmp[21];

        for (int y = 0; y < 21; y++)
            tmp[y] = this->get_bit(0,y);
        for (int x = 0; x < 23; x++)
        {
            for (int y = 0; y < 21; y++)
            {
                this->set_bit(x,y, this->get_bit(x+1,y));
            }
        }
        for (int y = 0; y < 21; y++)
            this->set_bit(23,y, tmp[y]);
    }
    void slide_right(){
        bool tmp[21];

        for (int y = 0; y < 21; y++)
            tmp[y] = this->get_bit(23,y);
        for (int x = 23; x > 0; x--)
        {
            for (int y = 0; y < 21; y++)
            {
                this->set_bit(x,y, this->get_bit(x-1,y));
            }
        }
        for (int y = 0; y < 21; y++)
            this->set_bit(0,y, tmp[y]);
    }
    void flip_left()
    {
        bool tmp, tmp2;
        if (this->opt->data.value("sprites").toArray().at(id).toObject().value("mc_mode").toBool())
        {
            for (int x = 0; x < 6; x++)
            {
                for (int y = 0; y < 21; y++)
                {
                    tmp = this->get_bit(2*x,y);
                    tmp2 = this->get_bit(2*x+1,y);
                    this->set_bit(2*x+1,y, this->get_bit(23-2*x,y));
                    this->set_bit(2*x,y, this->get_bit(23-2*x-1,y));
                    this->set_bit(23-2*x,y,tmp2);
                    this->set_bit(23-2*x-1,y,tmp);
                }
            }
        }
        else
        {
            for (int x = 0; x < 12; x++)
            {
                for (int y = 0; y < 21; y++)
                {
                    tmp = this->get_bit(x,y);
                    this->set_bit(x,y, this->get_bit(23-x,y));
                    this->set_bit(23-x,y,tmp);
                }
            }
        }
    }
    void flip_top()
    {
        bool tmp;
        for (int y = 0; y < 10; y++)
        {
            for (int x = 0; x < 24; x++)
            {
                tmp = this->get_bit(x,y);
                this->set_bit(x,y, this->get_bit(x,20-y));
                this->set_bit(x,20-y,tmp);
            }
        }
    }




    void reflect_top()
    {
        for (int y = 11; y <= 20; y++)
        {
            for (int x = 0; x < 24; x++)
            {
                this->set_bit(x,y, this->get_bit(x,20-y));
            }
        }
    }
    void reflect_left()
    {
        if (this->opt->data.value("sprites").toArray().at(id).toObject().value("mc_mode").toBool())
        {
            for (int x = 6; x < 12; x++)
            {
                for (int y = 0; y < 21; y++)
                {
                    this->set_bit(2*x+1,y, this->get_bit(23-2*x,y));
                    this->set_bit(2*x,y, this->get_bit(23-2*x-1,y));
                }
            }
        }
        else
        {
            for (int x = 12; x <= 23; x++)
            {
                for (int y = 0; y < 21; y++)
                {
                    this->set_bit(x,y, this->get_bit(23-x,y));
                }
            }
        }
    }

    void switch_col_to_mc1()
    {
        if (this->opt->data.value("sprites").toArray().at(id).toObject().value("mc_mode").toBool())
        {
            for (int x = 0; x < 12; x++)
            {
                for (int y = 0; y < 21; y++)
                {
                    if (this->get_bit(2*x,y) && !this->get_bit(2*x+1,y))
                    {
                        this->set_bit(2*x,y, false);
                        this->set_bit(2*x+1,y, true);
                    }
                    else if (!this->get_bit(2*x,y) && this->get_bit(2*x+1,y))
                    {
                        this->set_bit(2*x,y, true);
                        this->set_bit(2*x+1,y, false);
                    }
                }
            }
        }
    }


    void switch_col_to_mc2()
    {
        if (this->opt->data.value("sprites").toArray().at(id).toObject().value("mc_mode").toBool())
        {
            for (int x = 0; x < 12; x++)
            {
                for (int y = 0; y < 21; y++)
                {
                    if (this->get_bit(2*x,y) && !this->get_bit(2*x+1,y))
                    {
                        this->set_bit(2*x,y, true);
                        this->set_bit(2*x+1,y, true);
                    }
                    else if (this->get_bit(2*x,y) && this->get_bit(2*x+1,y))
                    {
                        this->set_bit(2*x,y, true);
                        this->set_bit(2*x+1,y, false);
                    }
                }
            }
        }
    }


    void switch_mc1_to_mc2()
    {
        if (this->opt->data.value("sprites").toArray().at(id).toObject().value("mc_mode").toBool())
        {
            for (int x = 0; x < 12; x++)
            {
                for (int y = 0; y < 21; y++)
                {
                    if (this->get_bit(2*x,y) && this->get_bit(2*x+1,y))
                    {
                        this->set_bit(2*x,y, false);
                        this->set_bit(2*x+1,y, true);
                    }
                    else if (!this->get_bit(2*x,y) && this->get_bit(2*x+1,y))
                    {
                        this->set_bit(2*x,y, true);
                        this->set_bit(2*x+1,y, true);
                    }
                }
            }
        }
    }


    void flood_fill(QPointF pos)
    {
        bool expand_x = this->opt->data.value("sprites").toArray().at(id).toObject().value("exp_x").toBool();
        bool expand_y = this->opt->data.value("sprites").toArray().at(id).toObject().value("exp_y").toBool();

        if (this->opt->data.value("sprites").toArray().at(id).toObject().value("mc_mode").toBool())
        {
            //multi col
            int w = 20 * (expand_y ? 0.5 : 1);
            int h = 10 * (expand_x ? 0.5 : 1);

            int x = pos.x()/w;
            int y = pos.y()/h;


            QList<QPoint> points;
            bool from_col_1 = this->get_bit(2*x,y);
            bool from_col_2 = this->get_bit(2*x+1,y);
            bool to_col_1 = true;
            bool to_col_2 = true;

            if ((left_pressed && opt->left_button == TRANSPARENT) ||
                (right_pressed && opt->right_button == TRANSPARENT))
            {
                to_col_1 = false;
                to_col_2 = false;
            }
            else if ((left_pressed && opt->left_button == COLOR) ||
                     (right_pressed && opt->right_button == COLOR))
            {
                to_col_1 = true;
                to_col_2 = false;
            }
            else if ((left_pressed && opt->left_button == MC1) ||
                     (right_pressed && opt->right_button == MC1))
            {
                to_col_1 = false;
                to_col_2 = true;
            }
            else if ((left_pressed && opt->left_button == MC2) ||
                     (right_pressed && opt->right_button == MC2))
            {
                to_col_1 = true;
                to_col_2 = true;
            }


            if (from_col_1 == to_col_1 && from_col_2 == to_col_2) return; // nothing to do
            points << QPoint(x,y);
            this->set_bit(2*x,y,to_col_1);
            this->set_bit(2*x+1,y,to_col_2);

            do
            {
                x = points.first().x();
                y = points.first().y();
                points.removeFirst();

                if (x > 0 && this->get_bit(2*x-2,y) == from_col_1 && this->get_bit(2*x-1,y) == from_col_2 && !points.contains(QPoint(x-1,y)))
                {
                    points << QPoint(x-1,y);
                    this->set_bit(2*x-2,y,to_col_1);
                    this->set_bit(2*x-1,y,to_col_2);
                }
                if (x < 11 && this->get_bit(2*x+2,y) == from_col_1 && this->get_bit(2*x+3,y) == from_col_2 && !points.contains(QPoint(x+1,y)))
                {
                    points << QPoint(x+1,y);
                    this->set_bit(2*x+2,y,to_col_1);
                    this->set_bit(2*x+3,y,to_col_2);
                }
                if (y > 0 && this->get_bit(2*x,y-1) == from_col_1 && this->get_bit(2*x+1,y-1) == from_col_2 && !points.contains(QPoint(x,y-1)))
                {
                    points << QPoint(x,y-1);
                    this->set_bit(2*x,y-1,to_col_1);
                    this->set_bit(2*x+1,y-1,to_col_2);
                }

                if (y < 20 && this->get_bit(2*x,y+1) == from_col_1 && this->get_bit(2*x+1,y+1) == from_col_2 && !points.contains(QPoint(x,y+1)))
                {
                    points << QPoint(x,y+1);
                    this->set_bit(2*x,y+1,to_col_1);
                    this->set_bit(2*x+1,y+1,to_col_2);
                }
            }
            while(points.count() > 0);
        }
        else
        {
            //single col
            int w = 10 * (expand_y ? 0.5 : 1);
            int h = 10 * (expand_x ? 0.5 : 1);

            int x = pos.x()/w;
            int y = pos.y()/h;

            QList<QPoint> points;
            bool from_col = this->get_bit(x,y);
            bool to_col = true;
            if ((left_pressed && opt->left_button == TRANSPARENT) ||
                (right_pressed && opt->right_button == TRANSPARENT))
            {
                to_col = false;
            }
            //else true


            if (from_col == to_col) return; // nothing to do
            points << QPoint(x,y);
            this->set_bit(x,y,to_col);

            do
            {
                x = points.first().x();
                y = points.first().y();
                points.removeFirst();

                if (x > 0 && this->get_bit(x-1,y) == from_col && !points.contains(QPoint(x-1,y)))
                {
                    points << QPoint(x-1,y);
                    this->set_bit(x-1,y,to_col);
                }
                if (x < 23 && this->get_bit(x+1,y) == from_col && !points.contains(QPoint(x+1,y)))
                {
                    points << QPoint(x+1,y);
                    this->set_bit(x+1,y,to_col);
                }
                if (y > 0 && this->get_bit(x,y-1) == from_col && !points.contains(QPoint(x,y-1)))
                {
                    points << QPoint(x,y-1);
                    this->set_bit(x,y-1,to_col);
                }
                if (y < 20 && this->get_bit(x,y+1) == from_col && !points.contains(QPoint(x,y+1)))
                {
                    points << QPoint(x,y+1);
                    this->set_bit(x,y+1,to_col);
                }
            }
            while(points.count() > 0);
        }
    }

private:
    int id;
    bool overlay_next = false;

    bool left_pressed = false;
    bool right_pressed = false;

    QSettings settings;
    options *opt;
};

#endif // SPRITE_H
