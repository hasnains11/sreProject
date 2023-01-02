#ifndef GROUPTREEITEM_H
#define GROUPTREEITEM_H

#include <QList>
#include <QObject>
#include <QVariant>

class GroupTreeModel;

class GroupTreeItem {
    friend class GroupTreeModel;
public:
    explicit GroupTreeItem(const QString &item_name,
                           const QString &item_domain,
                           GroupTreeItem *parentItem = 0);
    ~GroupTreeItem();

    void appendChild(GroupTreeItem *child);

    GroupTreeItem *child(int row);
    int childCount() const;
    int columnCount() const;
    QVariant data(int column) const;
    int row() const;
    GroupTreeItem *parentItem();
    void removeChild();
    QString getPathToRoot();
    const QString& getDomain() const;
    const QString& getName() const;
private:
    bool fake_for_load;
    QList<GroupTreeItem*> m_child_items;
    const QString m_item_name;
    const QString m_item_domain;
    GroupTreeItem *m_parent_item;
};
#endif // GROUPTREEITEM_H
