PerfTester::Application.routes.draw do
  root "benchmarks#revision_list"

  get "revisions", to: "benchmarks#revision_list", as: "revision_list"
  get "revisions/compare/:id", to: "benchmarks#select_right_side_for_compare", as: "select_rhs_compare"
  get "revisions/compare/:left_id/:right_id", to: "benchmarks#revision_compare", as: "do_compare"

  get "measurements/:id", constraints: {:id => /\d+/}, to: "benchmarks#measurements_list", as: "measurements_list"
  get "start", to: "benchmarks#new_revision", as: "new_revision"
  post "start", to: "benchmarks#run_benchmarks", as: "run_benchmarks"

  # The priority is based upon order of creation: first created -> highest priority.
  # See how all your routes lay out with "rake routes".

  # You can have the root of your site routed with "root"
  # root 'welcome#index'

  # Example of regular route:
  #   get 'products/:id' => 'catalog#view'

  # Example of named route that can be invoked with purchase_url(id: product.id)
  #   get 'products/:id/purchase' => 'catalog#purchase', as: :purchase

  # Example resource route (maps HTTP verbs to controller actions automatically):
  #   resources :products

  # Example resource route with options:
  #   resources :products do
  #     member do
  #       get 'short'
  #       post 'toggle'
  #     end
  #
  #     collection do
  #       get 'sold'
  #     end
  #   end

  # Example resource route with sub-resources:
  #   resources :products do
  #     resources :comments, :sales
  #     resource :seller
  #   end

  # Example resource route with more complex sub-resources:
  #   resources :products do
  #     resources :comments
  #     resources :sales do
  #       get 'recent', on: :collection
  #     end
  #   end
  
  # Example resource route with concerns:
  #   concern :toggleable do
  #     post 'toggle'
  #   end
  #   resources :posts, concerns: :toggleable
  #   resources :photos, concerns: :toggleable

  # Example resource route within a namespace:
  #   namespace :admin do
  #     # Directs /admin/products/* to Admin::ProductsController
  #     # (app/controllers/admin/products_controller.rb)
  #     resources :products
  #   end
end
