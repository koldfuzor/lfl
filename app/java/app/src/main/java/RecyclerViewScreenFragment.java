package com.lucidfusionlabs.app;

import java.util.ArrayList;
import java.util.List;

import android.annotation.SuppressLint;
import android.app.Fragment;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.LinearLayout;
import android.support.v7.widget.RecyclerView;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.DefaultItemAnimator;
import android.support.v7.widget.DividerItemDecoration;

public class RecyclerViewScreenFragment extends ScreenFragment {
    public View toolbar;
    public RecyclerView recyclerview;
    public ModelItemRecyclerViewAdapter adapter;
    public static String PARENT_SCREEN_ID = "PARENT_SCREEN_ID";
    public static String TOOLBAR_SCREEN_ID = "TOOLBAR_SCREEN_ID";

    public static final RecyclerViewScreenFragment newInstance(TableScreen parent) {
        RecyclerViewScreenFragment ret = new RecyclerViewScreenFragment();
        Bundle bundle = new Bundle(2);
        bundle.putInt(PARENT_SCREEN_ID, parent.screenId);
        bundle.putInt(TOOLBAR_SCREEN_ID, 0);
        ret.setArguments(bundle);
        ret.adapter = parent.model;
        ret.parent_screen = parent;
        return ret;
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if (MainActivity.screens.next_screen_id == 1) return;
        int parent_screen_id = getArguments().getInt(PARENT_SCREEN_ID);
        if (parent_screen_id < MainActivity.screens.screens.size()) {
            if ((parent_screen = MainActivity.screens.screens.get(parent_screen_id)) instanceof TableScreen)
                adapter = ((TableScreen)parent_screen).model;
        } else Log.e("lfl", "invalid screen_id=" + parent_screen_id + " total=" + MainActivity.screens.screens.size());
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        LinearLayout layout = (LinearLayout)inflater.inflate(R.layout.listview_main, container, false);
        if (toolbar != null) {
            ViewGroup parent = (ViewGroup)toolbar.getParent();
            if (parent != null) parent.removeView(toolbar);
            layout.addView(toolbar, new LinearLayout.LayoutParams(LinearLayout.LayoutParams.MATCH_PARENT,
                                                                  LinearLayout.LayoutParams.WRAP_CONTENT,
                                                                  android.view.Gravity.BOTTOM));
        }
        LinearLayoutManager layoutmanager = new LinearLayoutManager(getContext());
        recyclerview = (RecyclerView)layout.findViewById(R.id.list);
        recyclerview.setHasFixedSize(true);
        recyclerview.setAdapter(adapter);
        recyclerview.setLayoutManager(layoutmanager);
        recyclerview.setItemAnimator(new DefaultItemAnimator());
        recyclerview.addItemDecoration
            (new DividerItemDecoration(recyclerview.getContext(), layoutmanager.getOrientation()));
        return layout;
    }
}
